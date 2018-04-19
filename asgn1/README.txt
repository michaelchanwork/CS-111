Compile the program using the command "make".
In the makefile, this defaults to make myshell, which runs:
flex lex.l
cc lex.yy.c myshell.c -lfl -o myshell

You can also use "make clean" to remove all compiled file.

This program is a lesser copy of the shell present in FreeBSD.

Enter the command "exit" without quotes when inside the shell to exit.
Commands like cat and wc which take user input when not given arguments may be exited with CTRL+D.
myshell accounts for nearly all(to my knowledge) errors which might take place given odd user input.
