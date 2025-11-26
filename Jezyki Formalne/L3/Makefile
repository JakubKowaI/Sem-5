all: calc

calc: bison.tab.c lex.yy.c
	$(CC) -o calc bison.tab.c lex.yy.c -lfl -lm

bison.tab.c bison.tab.h: bison.y
	bison -d bison.y

lex.yy.c: lex.l bison.tab.h
	flex lex.l

clean:
	rm -f calc bison.tab.c bison.tab.h lex.yy.c
