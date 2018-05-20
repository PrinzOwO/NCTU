all:scanner
scanner:lex.yy.c
	gcc lex.yy.c -lfl -o scanner
lex.yy.c:lexscanner.l
	flex lexscanner.l	
test:test.in
	./scanner test.in
clean: 
	rm -f lex.yy.c scanner
