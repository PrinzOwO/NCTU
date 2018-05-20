%{
/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Project 2 YACC sample
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"

extern "C"{
  int yyerror(const char *msg);
  extern int yylex(void);
}

extern int linenum;		/* declared in lex.l */
extern FILE *yyin;		/* declared by lex */
extern char *yytext;		/* declared by lex */
extern char buf[256];		/* declared in lex.l */
extern int yylex(void);

Symboltable st;
vector<Entry> gventry;
Entry gentry;
Type gtype;
vector<string> loopitstring;
int afterfunc = 0;

void init(){
  gventry.clear();
  gentry.init();
  gtype.init();
}

void addtoEntry(){
  if(!loopitstring.empty() && loopitstring.back() != "") st.table.top().valid.insert(loopitstring.back());
  for(auto &obj : gventry){
    st.table.top().add(obj);
  }
  init();
  afterfunc = 0;
}

%}

%union{
  char *str;
}

/* tokens */
%token <str> ARRAY
%token <str> BEG
%token <str> BOOLEAN
%token <str> DEF
%token <str> DO
%token <str> ELSE
%token <str> END
%token <str> FALSE
%token <str> FOR
%token <str> INTEGER
%token <str> IF
%token <str> OF
%token <str> PRINT
%token <str> READ
%token <str> REAL
%token <str> RETURN
%token <str> STRING
%token <str> THEN
%token <str> TO
%token <str> TRUE
%token <str> VAR
%token <str> WHILE

%token <str> ID
%token <str> OCTAL_CONST
%token <str> INT_CONST
%token <str> FLOAT_CONST
%token <str> SCIENTIFIC
%token <str> STR_CONST

%token <str> OP_ADD
%token <str> OP_SUB
%token <str> OP_MUL
%token <str> OP_DIV
%token <str> OP_MOD
%token <str> OP_ASSIGN
%token <str> OP_EQ
%token <str> OP_NE
%token <str> OP_GT
%token <str> OP_LT
%token <str> OP_GE
%token <str> OP_LE
%token <str> OP_AND
%token <str> OP_OR
%token <str> OP_NOT

%token <str> MK_COMMA
%token <str> MK_COLON
%token <str> MK_SEMICOLON
%token <str> MK_LPAREN
%token <str> MK_RPAREN
%token <str> MK_LB
%token <str> MK_RB

%type  <str> int_const
%type  <str> id_list

/* start symbol */
%start program
%%

program			: ID MK_SEMICOLON { 
                            st.add();
                            st.table.top().add(Entry($1, "program"));
                            st.table.top().entry.back().type.name = "void";
                          }
			  program_body
			  END ID {
			    st.print(); 
			    init();
			  }
			;

program_body		: opt_decl_list opt_func_decl_list compound_stmt
			;

opt_decl_list		: decl_list
			| /* epsilon */
			;

decl_list		: decl_list decl
			| decl
			;

decl			: VAR id_list MK_COLON scalar_type {
                            for(auto &obj : gventry){
                              if(obj.kind != "") continue;
                              obj.kind = "variable";
                              obj.type = gtype;
                            }
                            addtoEntry();
                          }
                          MK_SEMICOLON       /* scalar type declaration */
			| VAR id_list MK_COLON array_type {
			    for(auto &obj : gventry){
                              if(obj.kind != "") continue;
                              obj.kind = "variable";
                              obj.type = gtype;
                            }
			    addtoEntry();
			  }
			  MK_SEMICOLON        /* array type declaration */
			| VAR id_list MK_COLON literal_const {
			    for(auto &obj : gventry) {
			      obj.kind = "constant";
			      obj.type = gentry.type;
			      obj.attr = gentry.attr;
			    }
			    addtoEntry();
			  }
			  MK_SEMICOLON     /* const declaration */
			;
int_const	        : INT_CONST   { $$ = $1; }
			| OCTAL_CONST { $$ = $1; }
			;

literal_const		: int_const          { 
                            gentry.type.name = "integer";
                            gentry.attr.push_back(Type(string($1)));
                          }
			| OP_SUB int_const   { 
                            gentry.type.name = "integer";
                            gentry.attr.push_back(Type(string("-") + string($2)));
			  }
			| FLOAT_CONST        {
			    gentry.type.name = "real";
			    gentry.attr.push_back(Type(string($1))); 
			  }
			| OP_SUB FLOAT_CONST {
			    gentry.type.name = "real";
			    gentry.attr.push_back(Type(string("-") + string($2)));
			  }
			| SCIENTIFIC         { 
			    gentry.type.name = "real";
			    gentry.attr.push_back(Type(string($1)));
			  }
			| OP_SUB SCIENTIFIC  { 
                            gentry.type.name = "real";
                            gentry.attr.push_back(Type(string("-") + string($2)));
			  }
			| STR_CONST          { 
			    gentry.type.name = "string";
			    gentry.attr.push_back(Type(string($1)));
			  }
			| TRUE               {
			    gentry.type.name = "boolean";
			    gentry.attr.push_back(Type(string("true")));
			  }
			| FALSE              { 
			    gentry.type.name = "boolean";
			    gentry.attr.push_back(Type(string("false")));
			  }
			;

opt_func_decl_list	: func_decl_list
			| /* epsilon */
			;

func_decl_list		: func_decl_list func_decl
			| func_decl
			;

func_decl		: ID {
                            st.table.top().add(Entry($1, "function"));
                          }
                          MK_LPAREN {
                            gventry.clear();
                          }
                          opt_param_list MK_RPAREN {
                            gtype.init();
                          }
                          opt_type {
                            st.table.top().entry.back().type = gtype;
                          }
                          MK_SEMICOLON {
                            for(auto &obj : gventry){
                              st.table.top().entry.back().attr.push_back(obj.type);
                            }
                            afterfunc = 1;
                          }
			  compound_stmt
			  END ID{ init(); }
			;

opt_param_list		: param_list
			| /* epsilon */
			;

param_list		: param_list MK_SEMICOLON param
			| param
			;

param			: id_list MK_COLON {
                            gtype.init();
                          }
                          type {
                            for(auto &obj : gventry){
                              if(obj.kind != "") continue;
                              obj.kind = "parameter";
                              obj.type = gtype;
                            }

                          }
			;

id_list			: id_list MK_COMMA ID {
			    gventry.push_back(Entry(string($3), string("")));
                          }
			| ID {
			    gventry.push_back(Entry(string($1), string("")));
			  }
			;

opt_type		: MK_COLON type 
			| /* epsilon */ {
			    gtype.name = "void";
			  }
			;

type			: scalar_type
			| array_type
			;

scalar_type		: INTEGER { 
                            gtype.name = "integer"; 
                          }
			| REAL    { 
			    gtype.name = "real"; 
			  }
			| BOOLEAN { 
			    gtype.name = "boolean"; 
			  }
			| STRING  { 
			    gtype.name = "string"; 
			  }
			;

array_type		: ARRAY int_const TO int_const OF {
                            gtype.cap.push_back(atoi($4) - atoi($2) + 1);
                          }
                          type
			;

stmt			: compound_stmt
			| simple_stmt
			| cond_stmt
			| while_stmt
			| for_stmt
			| return_stmt
			| proc_call_stmt
			;

compound_stmt		: BEG {
                            st.add();
                            if(afterfunc) addtoEntry();
                          }
			  opt_decl_list
			  opt_stmt_list
			  END {
			    st.print();
			  }
			;

opt_stmt_list		: stmt_list
			| /* epsilon */
			;

stmt_list		: stmt_list stmt
			| stmt
			;

simple_stmt		: var_ref OP_ASSIGN boolean_expr MK_SEMICOLON
			| PRINT boolean_expr MK_SEMICOLON
			| READ boolean_expr MK_SEMICOLON
			;

proc_call_stmt		: ID{ init(); }
                          MK_LPAREN opt_boolean_expr_list MK_RPAREN MK_SEMICOLON
			;

cond_stmt		: IF boolean_expr THEN
			  opt_stmt_list
			  ELSE
			  opt_stmt_list
			  END IF
			| IF boolean_expr THEN opt_stmt_list END IF
			;

while_stmt		: WHILE boolean_expr DO
			  opt_stmt_list
			  END DO
			;

for_stmt		: FOR ID {
                            if(st.table.top().loopvalid.find(string($2)) != st.table.top().loopvalid.end()){
                              cout << "<Error> found in Line " << linenum << ": ";
                              cout << string("symbol " + string($2) + " is redeclared\n");
                              loopitstring.push_back(string(""));
                            }
                            else{
                              st.table.top().loopvalid.insert(string($2)); 
                              loopitstring.push_back(string($2));
                            }
                          }
                          OP_ASSIGN int_const TO int_const DO
			  opt_stmt_list
			  END DO {
			    if(loopitstring.back() != "") {
			      st.table.top().loopvalid.erase(string($2));
			      st.table.top().valid.erase(string($2));
			    }
			    loopitstring.pop_back();
			  }
			;

return_stmt		: RETURN boolean_expr MK_SEMICOLON
			;

opt_boolean_expr_list	: boolean_expr_list
			| /* epsilon */
			;

boolean_expr_list	: boolean_expr_list MK_COMMA boolean_expr
			| boolean_expr
			;

boolean_expr		: boolean_expr OP_OR boolean_term
			| boolean_term
			;

boolean_term		: boolean_term OP_AND boolean_factor
			| boolean_factor
			;

boolean_factor		: OP_NOT boolean_factor 
			| relop_expr
			;

relop_expr		: expr rel_op expr
			| expr
			;

rel_op			: OP_LT
			| OP_LE
			| OP_EQ
			| OP_GE
			| OP_GT
			| OP_NE
			;

expr			: expr add_op term
			| term
			;

add_op			: OP_ADD
			| OP_SUB
			;

term			: term mul_op factor
			| factor
			;

mul_op			: OP_MUL
			| OP_DIV
			| OP_MOD
			;

factor			: var_ref
			| OP_SUB var_ref
			| MK_LPAREN boolean_expr MK_RPAREN
			| OP_SUB MK_LPAREN boolean_expr MK_RPAREN
			| ID { init(); }
			  MK_LPAREN opt_boolean_expr_list MK_RPAREN
			| OP_SUB ID { init(); }
			  MK_LPAREN opt_boolean_expr_list MK_RPAREN
			| literal_const { init(); };
			;

var_ref			: ID { init(); }
			| var_ref dim
			;

dim			: MK_LB boolean_expr MK_RB
			;

%%

int yyerror(const char *msg )
{
	(void) msg;
	fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
	fprintf( stderr, "|--------------------------------------------------------------------------\n" );
	exit(-1);
}

