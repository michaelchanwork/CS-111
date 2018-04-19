#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
extern char **getline();

int main()
{
    int i, j, k, pid, status, inredir, outredir, inv, in, out, stdout2, numpipes, offset1, offset2;
    int *pipeloc, *pipes;
    char **args, **pargs0, **pargs1, **pargs2, **pargs3, **pargs4;
    char specchars[] = {'(',')','>','<','|','&',';','*'};

    while(1)
    {
        args = getline();
        inredir = -1;
        outredir = -1;
        numpipes = 0;
        inv = 0;

        //CHECK FOR SYMBOLS
        for(i = 0; args[i] != NULL; i++)
        {
            if(strcmp(args[i], "exit") == 0) exit(0);
            switch(args[i][0])
            {
                case '>':
                    if (outredir == -1 && i != 0)
                    {
                        outredir = i;
                    }
                    else inv = 1;
                    break;
                case '<':
                    if (inredir == -1 && i != 0)
                    {
                        inredir = i;
                    }
                    else inv = 1;
                    break;
                case '|':
                    if(i != 0)
                    {
                        numpipes++;
                        if(numpipes == 1)
                        {
                            pipeloc = (int*)malloc(sizeof(int));
                        } else
                        {
                            pipeloc = (int*)realloc(pipeloc, sizeof(pipeloc)+sizeof(int));
                        }
                        pipeloc[numpipes-1] = i;
                    } else inv = 1;
                    break;
            }
        }

        //Invalid input, Check for redirection "Missing name for redirection"
        if(outredir > 0)
        {
            for(i=0; i<strlen(specchars); i++)
            {
                if(args[outredir+1] == NULL || args[outredir+1][0] == specchars[i]) inv = 1;
            }
        }
        if(inredir > 0)
        {
            for(i=0; i<strlen(specchars); i++)
            {
                if(args[inredir+1] == NULL || args[inredir+1][0] == specchars[i]) inv = 1;
            }
        }
        if(numpipes > 0)
        {
            for(i=0; i<strlen(specchars); i++)
            {
                for(j=0; j<numpipes;j++)
                {
                    if(args[pipeloc[j]+1] == NULL || args[pipeloc[j]+1][0] == specchars[i]) inv = 1;
                }
            }
        }

        if(inv == 1)
        {
            printf("Invalid input\n");
            continue;
        }

        //initialize pipes array and set up pipes
        if(numpipes>0)
        {
            pipes = (int*)malloc(sizeof(int)*2*numpipes);
            for(i=0; i<numpipes; i++)
            {
                pipe(pipes+(2*i));
            }
        }

        //AFTER GOING THROUGH ARGUMENTS, Loop child processes
        for(i=0; i<numpipes+1; i++)
        {
            pid = fork();
            if(pid < 0)
            {
                printf("fork: %s\n",strerror(errno));
                exit(1);
            }
            else if(pid == 0)   //Child process
            {
            //checking redirection
            if(outredir > 0)
            {
                if((stdout2 = dup(STDOUT_FILENO))<0)
                {
                    printf("dup: %s\n",strerror(errno));
                }
                if((out = open(args[outredir+1], O_WRONLY | O_TRUNC | O_CREAT))<0)
                {
                    printf("open: %s\n",strerror(errno));
                }
                if(dup2(out, STDOUT_FILENO)<0)
                {
                    printf("dup2: %s\n",strerror(errno));
                }
                if(close(out)<0)
                {
                    printf("close: %s\n",strerror(errno));
                }
            }
            //checking redirection
            if(inredir > 0)
            {
                if((in = open(args[inredir+1], O_RDONLY))<0)
                {
                    printf("open: %s\n",strerror(errno));
                }
                if(dup2(in, STDIN_FILENO)<0)
                {
                    printf("dup2: %s\n",strerror(errno));
                }
                if(close(in)<0)
                {
                    printf("close: %s\n",strerror(errno));
                }
            }

            //DUP ACCORDING PIPE(S)
            if(numpipes>0)
            {
                if(i == 0)
                {
                    dup2(pipes[1], STDOUT_FILENO);
                    for(j=0;j<numpipes*2;j++)
                    {
                        close(pipes[j]);
                    }
                } else if(i == numpipes)
                {
                    offset1 = 2*i-2;
                    dup2(pipes[offset1], STDIN_FILENO);
                    for(j=0;j<numpipes*2;j++)
                    {
                        close(pipes[j]);
                    }
                } else
                {
                    offset1 = 2*i-2;
                    offset2 = 2*i+1;
                    dup2(pipes[offset1], STDIN_FILENO);
                    dup2(pipes[offset2], STDOUT_FILENO);
                    for(j=0;j<numpipes*2;j++)
                    {
                        close(pipes[j]);
                    }
                }
            }
            //Check i loop to set j, j is starting condition of for loops
            k = 0;
            if(i == 0)
            {
                j = 0;
            } else
            {
                j = pipeloc[i-1]+1;
            }

            //initialize array when necessary
            switch(i)
            {
                case 0:
                    pargs0 = (char**)malloc(sizeof(char));
                    break;
                case 1:
                    pargs1 = (char**)malloc(sizeof(char));
                    break;
                case 2:
                    pargs2 = (char**)malloc(sizeof(char));
                    break;
                case 3:
                    pargs3 = (char**)malloc(sizeof(char));
                    break;
                case 4:
                    pargs4 = (char**)malloc(sizeof(char));
                    break;
            }

            //Check if final loop to set end condition
            if(i == numpipes)
            {
                //add valid arguments to correct pargs according to i
                for(; args[j] != NULL; j++)
                {
                    if(outredir > 0 && (j == outredir || j == outredir + 1)) continue;
                    if(inredir > 0 && (j == inredir || j == inredir + 1)) continue;
                    switch(i)
                    {
                        case 0:
                            pargs0 = (char**)realloc(pargs0, sizeof(pargs0)+sizeof(args[j]));
                            pargs0[k] = args[j];
                            k++;
                            break;
                        case 1:
                            pargs1 = (char**)realloc(pargs1, sizeof(pargs1)+sizeof(args[j]));
                            pargs1[k] = args[j];
                            k++;
                            break;
                        case 2:
                            pargs2 = (char**)realloc(pargs2, sizeof(pargs2)+sizeof(args[j]));
                            pargs2[k] = args[j];
                            k++;
                            break;
                        case 3:
                            pargs3 = (char**)realloc(pargs3, sizeof(pargs3)+sizeof(args[j]));
                            pargs3[k] = args[j];
                            k++;
                            break;
                        case 4:
                            pargs4 = (char**)realloc(pargs4, sizeof(pargs4)+sizeof(args[j]));
                            pargs4[k] = args[j];
                            k++;
                            break;
                    }
                }
            } else
            {
                //add valid arguments to correct pargs according to i
                for(; j < pipeloc[i]; j++)
                {
                    if(outredir > 0 && (j == outredir || j == outredir + 1)) continue;
                    if(inredir > 0 && (j == inredir || j == inredir + 1)) continue;
                    switch(i)
                    {
                        case 0:
                            pargs0 = (char**)realloc(pargs0, sizeof(pargs0)+sizeof(args[j]));
                            pargs0[k] = args[j];
                            k++;
                            break;
                        case 1:
                            pargs1 = (char**)realloc(pargs1, sizeof(pargs1)+sizeof(args[j]));
                            pargs1[k] = args[j];
                            k++;
                            break;
                        case 2:
                            pargs2 = (char**)realloc(pargs2, sizeof(pargs2)+sizeof(args[j]));
                            pargs2[k] = args[j];
                            k++;
                            break;
                        case 3:
                            pargs3 = (char**)realloc(pargs3, sizeof(pargs3)+sizeof(args[j]));
                            pargs3[k] = args[j];
                            k++;
                            break;
                    }
                }
            }
            //Check which array of args to execute with
            switch(i)
                {
                case 0:
                    execvp(pargs0[0], pargs0);
                    break;
                case 1:
                    execvp(pargs1[0], pargs1);
                    break;
                case 2:
                    execvp(pargs2[0], pargs2);
                    break;
                case 3:
                    execvp(pargs3[0], pargs3);
                    break;
                case 4:
                    execvp(pargs4[0], pargs4);
                    break;
            }
            //Check if need to "undup" to print error
            if(outredir > 0)
            {
                dup2(stdout2, STDOUT_FILENO);
                close(stdout2);
            }
            //Check which array of args for error
            switch(i)
                {
                case 0:
                    printf("%s: %s\n", pargs0[0], strerror(errno));
                    break;
                case 1:
                    printf("%s: %s\n", pargs1[0], strerror(errno));
                    break;
                case 2:
                    printf("%s: %s\n", pargs2[0], strerror(errno));
                    break;
                case 3:
                    printf("%s: %s\n", pargs3[0], strerror(errno));
                    break;
                case 4:
                    printf("%s: %s\n", pargs4[0], strerror(errno));
                    break;
            }
            exit(0);
            }
        }
        {   //Parent Process closes all pipes for safety
            if(numpipes > 0)
            {
                for(j=0;j<numpipes*2;j++)
                {
                    close(pipes[j]);
                }
            }
            if(wait(&status) < 0)  //Parent waiting for Child to finish
            {
                printf("Child process: %s\n",strerror(errno));
            }
        }

    }
}
