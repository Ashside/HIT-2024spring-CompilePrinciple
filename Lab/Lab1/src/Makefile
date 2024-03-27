all: lex.yy.c Parse.tab.h

Parse.tab.h: Parse.y
	bison -d Parse.y

lex.yy.c: Lexer.l
	flex Lexer.l

clean:
	rm -f lex.yy.c Parse.tab.h Parse.tab.c