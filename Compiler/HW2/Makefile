TARGET = y.tab.c lex.yy.c
all: parser

parser: $(TARGET)
	gcc $(TARGET) -ly -ll -o $@

lex.yy.c: lex.l y.tab.h
	flex lex.l 

y.tab.c: yacc.y
	yacc -dv yacc.y

clean:
	rm -f y.output y.tab.c y.tab.h lex.yy.c parser
