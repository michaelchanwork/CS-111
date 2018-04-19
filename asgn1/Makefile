#Makefile for asgn1
#Make a shell executable out of myshell.c and lex.l

myshell: myshell.c lex.yy.c
	gcc lex.yy.c myshell.c -lfl -o myshell.exe

lex.yy.c: lex.l
	flex lex.l

clean:
	rm -f lex.yy.c myshell.exe

.PHONY: clean
