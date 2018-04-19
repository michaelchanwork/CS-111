Compile the program using the command "make".
In the makefile, this defaults to make catpgrm, which runs:
mingw32-gcc.exe catpgrm.c -o catpgrm
You can also use "make clean" to remove the executable.

This program is a lesser copy of the system call "cat".
Usage:  catpgrm file1 file2 ...
            reads from files designated by file1 file2 etc... then writes them to stdout.
        catpgrm
            reads from stdin, pressing enter writes it back to stdout.
