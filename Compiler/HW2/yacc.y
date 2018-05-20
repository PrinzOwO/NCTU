%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
%}

%token ARRAY OF PRINT READ VAR
%token BG DO ELSE END FOR IF THEN TO RETURN WHILE
%token BOOL FALSE INT REAL STRING TRUE IDENT FLOAT SCI TYPE

%right ASSIGN

%left OR
%left AND
%left NOT
%left LT LE GT GE '=' NE
%left '+' '-'
%left '*' '/' MOD
%left NEG

%%

program		: programname ';' programbody END IDENT
		;

programname	: identifier
		;

programbody     : variables 
                  functions
                  compound_stmt
                ;

identifier	: IDENT
		;


ident_list      :identifier sub_ident_list
                ;

sub_ident_list  : ',' ident_list
                | /* epsilon */
                ;


functions       : function functions
                | /* epsilon */
                ;

function        : identifier '(' formal_argvs ')' func_ret_type ';' 
                    compound_stmt
                  END identifier
                ;

func_ret_type   : ':' func_type
                | /* epsilon */
                ;

func_type       : TYPE
                | array_type
                ;

formal_argvs    : formal_argv 
                | /* epsilon */
                ; 

formal_argv     : ident_list ':' TYPE sub_formal_argv
                ;

sub_formal_argv : ';' formal_argv
                | /* epsilon */
                ;

variables       : nonemptyvar
                | /* epsilon */
                ;

nonemptyvar     : variable nonemptyvar
                | /* epsilon */
                ;

variable        : var_declaration
                | const_declar
                ;

var_declaration : prefix_declar var_declar_type ';'
                ;

var_declar_type : var_type
                | array_type 
                ;

array_type      : ARRAY INT TO INT OF var_type
                ;

var_type        : TYPE
                | FLOAT
                | SCI
                | array_type
                ;

const_declar    : prefix_declar literal_const ';'
                ;

prefix_declar   : VAR ident_list ':'
                ;

literal_const   : INT
                | STRING
                | bool_const
                ;

bool_const      : TRUE
                | FALSE
                ;

func_const      : identifier '(' comma_expr ')'
                ;

statements      : nonemptystmt
                | /* epsilon */
                ;

nonemptystmt    : stmt nonemptystmt
                | /* epsilon */
                ;

stmt            : compound_stmt
                | simple_stmt
                | expression
                | condition_stmr
                | while_stmp
                | for_stmt
                | return_stmt
                | func_invocation
                ;

compound_stmt   : BG 
                    variables 
                    statements
                  END
                ;

simple_stmt     : var_refer ASSIGN expression ';'
                | PRINT print_sim_stmt ';'
                | READ var_refer ';'
                ;

print_sim_stmt  : var_refer
                | expression
                | func_const
                ;

var_refer       : identifier sub_var_refe
                ;

sub_var_refe    : '[' expression ']' sub_var_refe
                | /* epsilon */
                ;

expression      : '-' expr_type %prec NEG
                | expr_type '+' expression
                | expr_type '-' expression
                | expr_type '*' expression
                | expr_type '/' expression
                | expr_type MOD expression
                | expr_type LT expression
                | expr_type LE expression
                | expr_type '=' expression
                | expr_type GT expression
                | expr_type GE expression
                | expr_type NE expression
                | expr_type NOT expression
                | expr_type AND expression
                | expr_type OR expression
                | expr_type
                ;


expr_type       : literal_const
                | func_const
                | var_refer
                | '(' expression ')'
                | FLOAT
                | SCI
                ;

condition_stmr  : IF expression THEN
                    statements  
                  sub_condition
                ;

sub_condition   : ELSE 
                    statements 
                  END IF
                | statements 
                  END IF
                ;

while_stmp      : WHILE expression DO
                    statements
                  END DO
                ;

for_stmt        : FOR identifier ASSIGN INT TO INT DO
                    statements
                  END DO
                ;

return_stmt     : RETURN expression ';'
                ;

func_invocation : identifier '(' comma_expr ')' ';'
                ;

comma_expr      : sub_comma_expr
                | /* epsilon */
                ;

sub_comma_expr  : expression ssb_comma_expr
                ;

ssb_comma_expr  : ',' sub_comma_expr
                | /* epsilon */
                ;

%%

int yyerror( char *msg )
{
        fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
        fprintf( stderr, "|--------------------------------------------------------------------------\n" );
        exit(-1);
}

int  main( int argc, char **argv )
{
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
		exit(0);
	}

	FILE *fp = fopen( argv[1], "r" );
	
	if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}
	
	yyin = fp;
	yyparse();

	fprintf( stdout, "\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic error!  |\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	exit(0);
}

