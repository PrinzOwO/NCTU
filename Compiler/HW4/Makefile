TARGET   = parser
OBJECT   = y.tab.c lex.yy.c symboltable.o semantic.o y.output parser y.tab.h
CC       = g++
CFLAGS   = -Wall -Wextra -pedantic -g3
LEX      = flex
YACC     = yacc -v -t
YACCFLAG = -d
LIBS     = -lfl
RM       = rm

all: $(TARGET)
	
$(TARGET): y.tab.c lex.yy.c symboltable.o semantic.o main.cpp
	$(CC) $^ -o $(TARGET) $(CCFLAGS) $(LIBS)

y.tab.c: parser.y
	$(YACC) $(YACCFLAG) $<

lex.yy.c: lex.l
	$(LEX) $<

symboltable.o: symboltable.cpp
	$(CC) $< -c -o $@

semantic.o: semantic.cpp
	$(CC) $< -c -o $@

.PHONY: clean

clean:
	$(RM) -f $(TARGET) $(OBJECT)
