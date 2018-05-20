#include <iostream>
#include <vector>
#include <stack>
#include "symboltable.h"

using namespace std;

#ifndef __SEMANTIC__
#define __SEMANTIC__

namespace SemanticError{
  void filename(int, string);
  void programname(int);
  void funcname(int);
  void varnotdeclared(int, string);
  void constassign(int, string);
  void arrayassign(int);
  void arrayreturn(int);
  void arrayinvalid(int);
  void opunary(int);
  void opnoadd(int, char *);
  void opstr(int, char *);
  void opmod(int);
  void oprel(int, char *);
  void opboolean(int, char *);
  void mismatch(int, string, string);
  void paranum(int, string, string);
  void paramismatch(int);
  void errortype(int, string);
};

struct VarInfo{
  Entry *info;
  string name;
  Type type;
  int dim;
  
  void init();
  VarInfo(Entry *info = NULL, string name = "", int dim = 0, Type type = Type()) : info(info), name(name), dim(dim), type(type) {}
};

struct SemanticDef{
  stack<Type> block;
  
  void check(string);
};

#endif
