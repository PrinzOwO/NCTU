%{
/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Project 2 YACC sample
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"
#include "semantic.h"
#include "define.h"

extern "C"{
  int yyerror(const char *msg);
  extern int yylex(void);
}

extern char *filename;          /* declared in main.c */
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
  if(!loopitstring.empty() && loopitstring.back() != "") st.table.back().valid.insert(loopitstring.back());
  for(auto &obj : gventry){
    st.table.back().add(obj);
  }
  init();
  afterfunc = 0;
}

// Hw4 declare
string fname;                   /* Clipped Filename */
SemanticDef gsemanticdef;
vector<VarInfo> gvarinfo;
VarInfo gleftidtype;
int varerr = 0;
int paraidx;
int returnidx;
int dimidx;
int printidx;
int readidx;
int ifidx;

bool typecheck(VarInfo &a, VarInfo &b){
  if(a.type.name != b.type.name || a.dim != b.dim) return false;
  else{
    int flag = 1;
    int aback = a.type.cap.size();
    int bback = b.type.cap.size();
    for(int i = 0; i < a.dim; i++){
      if(aback - 1 - i < 0 || bback - 1 - i < 0){
        flag = 0;
        break;
      }
      if(a.type.cap[aback - 1 - i] != b.type.cap[bback - 1 - i]){
        flag = 0;
        break;
      }
    }
    if(flag) return true;
    else return false;
  }
}

void IDprocess(char *str){
  Entry *obj = st.find(string(str));
  if(obj == NULL){
    varerr = 1;
    SemanticError::varnotdeclared(linenum, string(str)); 
  }
  else{
    gvarinfo.push_back(VarInfo(obj, obj->name, obj->type.cap.size(), obj->type));
  }
  init(); 
}

void seterror(){
  gvarinfo.clear();
  gvarinfo.push_back(VarInfo(NULL, "", 0, Type("error")));
}

void checkneg(){
  if(varerr || gvarinfo.back().type.name == "error") return;
  if(gvarinfo.back().type.name != "integer" && gvarinfo.back().type.name != "real" || gvarinfo.back().dim){
    SemanticError::opunary(linenum); 
    seterror();
  }
}

void checknoadd(char *op){
  if(gvarinfo.size() < 2) return;  
  VarInfo tmp1 = gvarinfo.back();
  gvarinfo.pop_back();
  VarInfo tmp2 = gvarinfo.back();
  gvarinfo.pop_back();

 
  if(tmp1.type.name != "integer" && tmp1.type.name != "real" || tmp1.dim){
    if(tmp1.type.name == "string")
      if(typecheck(tmp1, tmp2)){
        SemanticError::opstr(linenum, op);
        gvarinfo.push_back(VarInfo(NULL, "", 0, Type(tmp1.type.name)));
        return;
      }
      else
        SemanticError::opstr(linenum, op);
    else
      SemanticError::opnoadd(linenum, op); 
    seterror();
    return;
  }

  if(tmp2.type.name != "integer" && tmp2.type.name != "real" || tmp2.dim){
    if(tmp2.type.name == "string")
      SemanticError::opstr(linenum, op);
    else
      SemanticError::opnoadd(linenum, op); 
    seterror();
    return;
  }

  if(tmp1.type.name == "integer" && tmp2.type.name == "integer")
    gvarinfo.push_back(VarInfo(NULL, "", 0, Type("integer")));
  else
    gvarinfo.push_back(VarInfo(NULL, "", 0, Type("real")));
}

void checkadd(){
  char *add = strdup("+");
  if(gvarinfo.size() < 2) return;
  VarInfo tmp1 = gvarinfo.back();
  gvarinfo.pop_back();
  if(tmp1.type.name != "string" || tmp1.dim){
    gvarinfo.push_back(tmp1);
    checknoadd(add);
    return;
  }

  VarInfo tmp2 = gvarinfo.back();
  gvarinfo.pop_back();
  if(tmp2.type.name != "string" || tmp2.dim){
    gvarinfo.push_back(tmp2);
    gvarinfo.push_back(tmp1);
    checknoadd(add);
    return;
  }
  
  gvarinfo.push_back(VarInfo(NULL, "", 0, Type("string"))); 
}

void checkmod(){
  if(gvarinfo.size() < 2) return;
  VarInfo tmp1 = gvarinfo.back();
  gvarinfo.pop_back();
  VarInfo tmp2 = gvarinfo.back();
  gvarinfo.pop_back();
  
  if(tmp1.type.name != "integer" || tmp1.dim){
    if(typecheck(tmp1, tmp2)){
      SemanticError::opmod(linenum);
      gvarinfo.push_back(VarInfo(NULL, "", 0, Type(tmp1.type.name)));
      return;
    }
    SemanticError::opmod(linenum);
    seterror();
    return;
  }

  if(tmp2.type.name != "integer" || tmp2.dim){
    SemanticError::opmod(linenum);
    seterror();
    return;
  }
  gvarinfo.push_back(VarInfo(NULL, "", 0, Type("integer")));
}

void checkrel(char *op){
  if(gvarinfo.size() < 2) return;
  VarInfo tmp1 = gvarinfo.back();
  gvarinfo.pop_back();
  VarInfo tmp2 = gvarinfo.back();
  gvarinfo.pop_back();

  if(tmp1.type.name != "integer" && tmp1.type.name != "real" || tmp1.dim){
    if(typecheck(tmp1, tmp2)){
      SemanticError::oprel(linenum, op);
      gvarinfo.push_back(VarInfo(NULL, "", 0, Type(tmp1.type.name)));
      return;
    }
    SemanticError::oprel(linenum, op);
    seterror();
    return;
  }

  if(tmp2.type.name != "integer" && tmp2.type.name != "real" || tmp2.dim){
    SemanticError::oprel(linenum, op);
    seterror();
    return;
  }
  gvarinfo.push_back(VarInfo(NULL, "", 0, Type("boolean")));
}

void checkboolean(char *op){
  if(gvarinfo.empty()) return;
  VarInfo tmp1 = gvarinfo.back();
  gvarinfo.pop_back();

  if(string(op) == "not") {
    if(tmp1.type.name != "boolean" || tmp1.dim){
      SemanticError::opboolean(linenum, op);
      gvarinfo.push_back(VarInfo(NULL, "", 0, Type(tmp1.type.name)));
    }
  }
  else{
    if(gvarinfo.empty()) return;
    VarInfo tmp2 = gvarinfo.back();
    gvarinfo.pop_back();

    if(tmp1.type.name != "boolean" || tmp1.dim){
      if(typecheck(tmp1, tmp2)){
        SemanticError::opboolean(linenum, op);
        gvarinfo.push_back(VarInfo(NULL, "", 0, Type(tmp1.type.name)));
        return;
      }
      SemanticError::opboolean(linenum, op);
      seterror();
      return;
    }

    if(tmp2.type.name != "boolean" || tmp2.dim){
      SemanticError::opboolean(linenum, op);
      seterror();
      return;
    }
    gvarinfo.push_back(VarInfo(NULL, "", 0, Type("boolean")));
  }
}

void checkparameter(char *in){
  Entry *obj = st.find(in);

  if(int(gvarinfo.size()) - paraidx > obj->attr.size())
    SemanticError::paranum(linenum, string(in), "many"); 
  else if(int(gvarinfo.size()) - paraidx < obj->attr.size())
    SemanticError::paranum(linenum, string(in), "few");

  if(gleftidtype.info != NULL){
    int flag = 0;
    for(int i = paraidx, j = 0; i < int(gvarinfo.size()); i++, j++){
      if(j >= obj->attr.size()) break;
      VarInfo tmp = VarInfo(NULL, "", obj->attr[j].cap.size(), obj->attr[j]);
      if(typecheck(tmp, gvarinfo[i]) == 0){
        SemanticError::paramismatch(linenum); 
        flag = 1;
      }
      if(flag) break;
    }
  }
  while(gvarinfo.size() > paraidx) gvarinfo.pop_back();
}

void checkassignexpr(){

  stringstream ss1;
  string tmp;
  string tmp1 = gleftidtype.type.name;
  for(int i = 0; i < gleftidtype.dim; i++) 
    ss1 << "[" << gleftidtype.type.cap[i] << "]";
  ss1 >> tmp;
  tmp1 += tmp;

  stringstream ss2;
  tmp.clear();
  string tmp2 = gvarinfo[0].type.name;
  for(int i = 0; i < gvarinfo[0].dim; i++) 
    ss2 << "[" << gvarinfo[0].type.cap[i] << "]";
  ss2 >> tmp;
  tmp2 += tmp;

  if(typecheck(gleftidtype, gvarinfo[0]) == 0){
    if(gvarinfo[0].type.name == "error"){
      SemanticError::errortype(linenum, "RHS");
    }
    else {
      if(tmp1 == tmp2 || (tmp1 == "real" && tmp2 == "integer")){
      // pass 
      }
      else
        SemanticError::mismatch(linenum, tmp1, tmp2);
    }
  } 
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
%type  <str> mul_op add_op rel_op

/* start symbol */
%start program
%%

program			: ID {
                            fname = string(filename);
                            while(fname.back() != '.') 
                              fname.pop_back();
                            fname.pop_back();
                            int front = int(fname.size()) - 1;
                            while(front >= 0 && fname[front] != '/') front--;
                            fname = fname.substr(front + 1, int(fname.size()) - 1 - front);

                            if(fname != string($1)) 
                              SemanticError::filename(linenum, "beginning");
                          }
                          MK_SEMICOLON { 
                            st.add();
                            st.table.back().add(Entry($1, "program"));
                            st.table.back().entry.back().type.name = "void";
                          }
			  program_body
			  END ID {
			    st.print(); 
			    init();
			    if(string($1) != string($7)) 
			      SemanticError::programname(linenum); 
			    if(fname != string($7)) 
                              SemanticError::filename(linenum, "end");
			    gvarinfo.clear();
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
			      obj.kind = string("constant");
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
                            gvarinfo.push_back(VarInfo(NULL, $1, 0, gentry.type));
                          }
			| OP_SUB int_const   { 
                            gentry.type.name = "integer";
                            gentry.attr.push_back(Type(string("-") + string($2)));
                            gvarinfo.push_back(VarInfo(NULL, $2, 0, gentry.type));
			  }
			| FLOAT_CONST        {
			    gentry.type.name = "real";
			    gentry.attr.push_back(Type(string($1))); 
                            gvarinfo.push_back(VarInfo(NULL, $1, 0, gentry.type));
			  }
			| OP_SUB FLOAT_CONST {
			    gentry.type.name = "real";
			    gentry.attr.push_back(Type(string("-") + string($2)));
                            gvarinfo.push_back(VarInfo(NULL, $2, 0, gentry.type));
			  }
			| SCIENTIFIC         { 
			    gentry.type.name = "real";
			    gentry.attr.push_back(Type(string($1)));
                            gvarinfo.push_back(VarInfo(NULL, $1, 0, gentry.type));
			  }
			| OP_SUB SCIENTIFIC  { 
                            gentry.type.name = "real";
                            gentry.attr.push_back(Type(string("-") + string($2)));
                            gvarinfo.push_back(VarInfo(NULL, $2, 0, gentry.type));
			  }
			| STR_CONST          { 
			    gentry.type.name = "string";
			    gentry.attr.push_back(Type(string($1)));
                            gvarinfo.push_back(VarInfo(NULL, $1, 0, gentry.type));
			  }
			| TRUE               {
			    gentry.type.name = "boolean";
			    gentry.attr.push_back(Type(string("true")));
                            gvarinfo.push_back(VarInfo(NULL, $1, 0, gentry.type));
			  }
			| FALSE              { 
			    gentry.type.name = "boolean";
			    gentry.attr.push_back(Type(string("false")));
                            gvarinfo.push_back(VarInfo(NULL, $1, 0, gentry.type));
			  }
			;

opt_func_decl_list	: func_decl_list
			| /* epsilon */
			;

func_decl_list		: func_decl_list func_decl
			| func_decl
			;

func_decl		: ID {
                            st.table.back().add(Entry($1, "function"));
                          }
                          MK_LPAREN {
                            gventry.clear();
                          }
                          opt_param_list MK_RPAREN {
                            gtype.init();
                          }
                          opt_type {
                            st.table.back().entry.back().type = gtype;
                            if(gtype.cap.size() > 0) 
                              SemanticError::arrayreturn(linenum); 
                          }
                          MK_SEMICOLON {
                            for(auto &obj : gventry){
                              st.table.back().entry.back().attr.push_back(obj.type);
                            }
                            afterfunc = 1;
			    gvarinfo.clear();
			    varerr = 0;
                          }
			  compound_stmt
			  END ID { 
			    init(); 
			    if(string($1) != string($14))
			      SemanticError::funcname(linenum);
			    gvarinfo.clear();
			    varerr = 0;
			  }
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
                            if(atoi($4) - atoi($2) > 0 && atoi($4) >= 0 && atoi($2) >= 0)
                              gtype.cap.push_back(atoi($4) - atoi($2) + 1);
                            else
                              SemanticError::arrayinvalid(linenum);
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
                            if(afterfunc) {
                              gsemanticdef.block.push(gtype);
                              addtoEntry();
                            }
                            else
                              gsemanticdef.block.push(Type());
                          }
			  opt_decl_list
			  opt_stmt_list
			  END {
			    st.print();
			    gvarinfo.clear();
			    gsemanticdef.block.pop();
			  }
			;

opt_stmt_list		: stmt_list
			| /* epsilon */
			;

stmt_list		: stmt_list stmt
			| stmt
			;

simple_stmt		: var_ref OP_ASSIGN {
                            if(!varerr){
                              if(gvarinfo[0].info->kind == string("constant")){
                                SemanticError::constassign(linenum, gvarinfo[0].name);
                              }
                              else if(gvarinfo[0].dim != 0){
                                SemanticError::arrayassign(linenum); 
                              }
                              gleftidtype = gvarinfo[0];
                            }
                            gvarinfo.clear();
                            varerr = 0;
                          }
                          boolean_expr MK_SEMICOLON{
                            checkassignexpr();
                            gleftidtype.init();
                            gvarinfo.clear();
                            varerr = 0;
                          }
			| PRINT {
			    printidx = gvarinfo.size();  
			  }
			  boolean_expr MK_SEMICOLON {
                            if(gvarinfo.size() <= printidx || gvarinfo[printidx].type.name == "error" || gvarinfo[printidx].dim){
                              cout << "<Error> found in Line "; 
                              cout << linenum << ": print statement is not scalar type" << endl; 
                            }
                            while(gvarinfo.size() > printidx) gvarinfo.pop_back();

			    gleftidtype.init();
                            gvarinfo.clear();
                            varerr = 0;
			  }
			| READ {
			    readidx = gvarinfo.size();
			  }
			  boolean_expr MK_SEMICOLON {
			    if(gvarinfo.size() <= readidx || gvarinfo[readidx].type.name == "error" || gvarinfo[readidx].dim){
                              cout << "<Error> found in Line "; 
                              cout << linenum << ": read statement is not scalar type" << endl; 
                            }
                            while(gvarinfo.size() > readidx) gvarinfo.pop_back();

			    gleftidtype.init();
                            gvarinfo.clear();
                            varerr = 0;
			  }
			;

proc_call_stmt		: ID{ init(); }
                          MK_LPAREN opt_boolean_expr_list MK_RPAREN MK_SEMICOLON {
                            gleftidtype.init();
                            gvarinfo.clear();
                            varerr = 0;
                          }
			;

// add by myself
else_stmt               : ELSE opt_stmt_list
                        | /* epsilon */
                        ;

cond_stmt		: IF boolean_expr THEN {
                            if(gvarinfo.size() <= ifidx || gvarinfo[ifidx].type.name == "error" || gvarinfo[ifidx].dim){
                              cout << "<Error> found in Line "; 
                              cout << linenum << ": condition statement is not scalar type" << endl; 
                            }
                            while(gvarinfo.size() > ifidx) gvarinfo.pop_back();

			    gleftidtype.init();
                            gvarinfo.clear();
                            varerr = 0;
                          }
			  opt_stmt_list
			  else_stmt
			  END IF {
			    gvarinfo.clear();
			    varerr = 0;
			  } 
			;

while_stmt		: WHILE {
                            gvarinfo.clear();
                            varerr = 0;
                          }
                          boolean_expr DO{
                            if(gvarinfo.back().type.name != "boolean" || gvarinfo.back().dim){
                              cout << "<Error> found in Line ";
                              cout << linenum;
                              cout << ": operand of while statement is not boolean type" << endl;
                            }
                            gleftidtype.init();
                            gvarinfo.clear();
                            varerr = 0;
                          }
			  opt_stmt_list
			  END DO{
                            gleftidtype.init();
			    gvarinfo.clear();
                            varerr = 0;
			  }
			;

for_stmt		: FOR ID {
                            if(st.table.back().loopvalid.find(string($2)) != st.table.back().loopvalid.end()){
                              cout << "<Error> found in Line " << linenum << ": ";
                              cout << string("symbol " + string($2) + " is redeclared\n");
                              loopitstring.push_back(string(""));
                            }
                            else{
                              st.table.back().loopvalid.insert(string($2)); 
                              loopitstring.push_back(string($2));
                            }
                          }
                          OP_ASSIGN int_const TO int_const DO {
                            if(atoi($7) < 0 || atoi($5) < 0) 
                              cout << "<Error> found in Line " << linenum << ": loop parameters must be greater than or equal to zero" << endl;
                            if(atoi($7) - atoi($5) <= 0)
                              cout << "<Error> found in Line " << linenum << ": loop parameters must be in the incremental order" << endl;
                          }
			  opt_stmt_list
			  END DO {
			    if(loopitstring.back() != "") {
			      st.table.back().loopvalid.erase(string($2));
			      st.table.back().valid.erase(string($2));
			    }
			    loopitstring.pop_back();
			    gvarinfo.clear();
			  }
			;

return_stmt		: RETURN {
                            returnidx = gvarinfo.size(); 
                          }
                          boolean_expr MK_SEMICOLON{
                            VarInfo tmp = VarInfo(NULL, "", gsemanticdef.block.top().cap.size(), gsemanticdef.block.top());
                            if(gvarinfo.size() <= returnidx || typecheck(gvarinfo[returnidx], tmp) == 0){
                              cout << "<Error> found in Line " << linenum << ": return type mismatch" << endl;
                            }
                            while(gvarinfo.size() > returnidx) gvarinfo.pop_back();

                          }
			;

opt_boolean_expr_list	: boolean_expr_list
			| /* epsilon */
			;

boolean_expr_list	: boolean_expr_list MK_COMMA boolean_expr
			| boolean_expr
			;

boolean_expr		: boolean_expr OP_OR boolean_term {
                            checkboolean($2);
                          }
			| boolean_term
			;

boolean_term		: boolean_term OP_AND boolean_factor{
                            checkboolean($2);
                          }
			| boolean_factor
			;

boolean_factor		: OP_NOT boolean_factor {
                            checkboolean($1);
                          }
			| relop_expr
			;

relop_expr		: expr rel_op expr {
                            checkrel($2);
                          }
			| expr
			;

rel_op			: OP_LT { $$ = $1; }
			| OP_LE { $$ = $1; }
			| OP_EQ { $$ = $1; }
			| OP_GE { $$ = $1; }
			| OP_GT { $$ = $1; }
			| OP_NE { $$ = $1; }
			;

expr			: expr add_op term {
                            if(string($2) == "+") {
                              checkadd();
                            }
                            else{
                              checknoadd($2);
                            }
                          }
			| term
			;

add_op			: OP_ADD { $$ = $1; }
			| OP_SUB { $$ = $1; }
			;

term			: term mul_op factor {
                            if(string($2) == "mod"){
                              checkmod(); 
                            }
                            else{
                              checknoadd($2);
                            }
                          }
			| factor
			;

mul_op			: OP_MUL { $$ = $1; }
			| OP_DIV { $$ = $1; }
			| OP_MOD { $$ = $1; }
			;

factor			: var_ref
			| OP_SUB var_ref {
			    checkneg();
			  }
			| MK_LPAREN boolean_expr MK_RPAREN
			| OP_SUB MK_LPAREN boolean_expr MK_RPAREN {
			    checkneg();
			  }
			| ID { 
			    IDprocess($1);
			  }
			  MK_LPAREN {
			    paraidx = gvarinfo.size();
			  }
			  opt_boolean_expr_list MK_RPAREN {
			    checkparameter($1);
                          }
			| OP_SUB ID { 
			    IDprocess($2);
			    checkneg();
			  }
			  MK_LPAREN 
			  opt_boolean_expr_list MK_RPAREN {
			    checkparameter($2);
			  }
			| literal_const { 
			    init(); 
			  };
			;

var_ref			: ID { 
                            IDprocess($1);
                          }
			| var_ref dim
			;

dim			: MK_LB {
                            dimidx = gvarinfo.size(); 
                          }
                          boolean_expr MK_RB{
                            if(gvarinfo.size() <= dimidx || gvarinfo[dimidx].type.name != "integer"){
                              cout << "<Error> found in Line "; 
                              cout << linenum << ": array index is not integer" << endl; 
                            }
                            while(gvarinfo.size() > dimidx) gvarinfo.pop_back();
			    if(!varerr) {
			      gvarinfo.back().dim--;
			    }
                          }
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

