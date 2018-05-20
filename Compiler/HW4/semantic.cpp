#include "semantic.h"

void SemanticError::filename(int linenum, string text){
    cout << "<Error> found in Line " << linenum << ": program " << text << " ID inconsist with file name" << endl;
}

void SemanticError::programname(int linenum){
  cout << "<Error> found in Line " << linenum << ": program end ID inconsist with the beginning ID" << endl;
}

void SemanticError::funcname(int linenum){
  cout << "<Error> found in Line " << linenum << ": the end of the functionName mismatch" << endl;
}

void SemanticError::varnotdeclared(int linenum, string text){
  cout << "<Error> found in Line " << linenum << ": '" << text << "' is not declared" << endl;
}

void SemanticError::constassign(int linenum, string text){
  cout << "<Error> found in Line " << linenum <<": constant '" << text << "' cannot be assigned" << endl;
}

void SemanticError::arrayassign(int linenum){
  cout << "<Error> found in Line " << linenum << ": array assignment is not allowed" << endl;
}

void SemanticError::arrayreturn(int linenum){
  cout << "<Error> found in Line " << linenum << ": a function cannot return an array type" << endl;
}

void SemanticError::arrayinvalid(int linenum){
  cout << "<Error> found in Line " << linenum << ": array index is invalid" << endl;
}

void SemanticError::opunary(int linenum){
  cout << "<Error> found in Line " << linenum << ": operand of unary - is not integer/real" << endl;
}
  
void SemanticError::opnoadd(int linenum, char *op){
  cout << "<Error> found in Line " << linenum <<": operands of operator '" << op << "' are not both integer or both real" << endl;
}

void SemanticError::opstr(int linenum, char *op){
  cout << "<Error> found in Line " << linenum << ": one of the operands of operator '" << op << "' is string type" << endl;
}

void SemanticError::mismatch(int linenum, string text1, string text2){
  cout << "<Error> found in Line " << linenum << ": type mismatch, LHS= " << text1 << ", RHS= " << text2 << endl;
}

void SemanticError::opmod(int linenum){
  cout << "<Error> found in Line " << linenum << ": one of the operands of operator 'mod' is not integer" << endl;
}

void SemanticError::oprel(int linenum, char *op){
  cout << "<Error> found in Line " << linenum << ": operands of operator '" << op << "' are not both integer or both real" << endl;
}

void SemanticError::opboolean(int linenum, char *op){
  if(string(op) != "not")
    cout << "<Error> found in Line " << linenum << ": one of the operands of operator '" << op << "' is not boolean" << endl;
  else
    cout << "<Error> found in Line " << linenum << ": operand of operator '" << op << "' is not boolean" << endl;

}

void SemanticError::errortype(int linenum, string text){
  cout << "<Error> found in Line " << linenum << ": error type in " << text << " of assignment" << endl;
}

void SemanticError::paranum(int linenum, string funcname, string text){
  cout << "<Error> found in Line " << linenum << ": too " << text << " arguments to function '" << funcname << "'" << endl;
}
  
void SemanticError::paramismatch(int linenum){
  cout << "<Error> found in Line " << linenum << ": parameter type mismatch" << endl;
}
/*
void SemanticDef::check(string type){
  auto &obj = this->kind;
  if(obj.top() == PROGRAM && tpye != "")
    
}
*/

void VarInfo::init(){
  this->info = NULL;
  this->name.clear();
  type.init();
  dim = 0;
}
