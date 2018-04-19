#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int read(int fd, void *buf, unsigned int nbytes);
    int write(int fd, const void *buf, unsigned int nbytes);
    int close(int fd);
    int loops, n = 0, fileno, rerr, werr, cerr; //initializations
	char* buffer;
	char* input;

    if(argc == 1)   //no arguments
    {
        buffer = (char*)malloc(sizeof(char));   //allocate memory for single chars
        input = (char*)malloc(sizeof(char));
        while(1)
        {
            rerr = read(0, buffer, 1);
            if(rerr == -1)
            {
                perror("Read failed");
            }
            if(rerr == 0)           //reads a char from stdin
            {
                if(n == 0)          //check for EOF
                {
                    free(buffer);
                    free(input);
                    return(0);
                }
                else
                {
                    werr = write(1,input,n+1); //writes out array
                    if(werr == -1)
                    {
                        perror("Write failed");
                    }
                    input = NULL;       //sets array to null pointer
                    n = -1;             //leads to memory allocated for 1 char
                }
            }
            else input[n] = buffer[0];   //adds to char array
            if(buffer[0] == '\n')   //check for new line(enter)
            {
                werr = write(1,input,n+1); //writes out array
                if(werr == -1)
                {
                    perror("Write failed");
                }
                input = NULL;       //sets array to null pointer
                n = -1;             //leads to memory allocated for 1 char
            }
            input = (char*) realloc(input, sizeof(char)*(++n+1));   //reallocates memory for +1 char
        }
        return 0;
    }
    else
        buffer = (char*) malloc(sizeof(char)*2048); //set buffer size for 2048 characters
        for(loops = 1; loops < argc; loops++)       //loop, once for each file/argument
            {
                fileno = open(argv[loops], O_RDONLY);   //open as read-only
                if(fileno==-1)                          //check for read error
                {
                    perror((char*)argv[loops]);
                }
                else
                {
                    do    //loop for reading until EOF
                    {
                        if((rerr = read(fileno, buffer, sizeof(buffer))) == -1)
                        {
                            perror("Read failed");
                        }
                        if((werr = write(1, buffer, rerr)) == -1)
                        {
                            perror("Write failed");
                        }
                    } while(rerr > 0);
                    if((cerr = close(fileno)) == -1)    //close and check for errors
                    {
                        perror("Closing file failed");
                    }
                }
            }
    free(buffer);
    return 0;
}
