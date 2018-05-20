#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <stack>
#include <vector>
#include <set>
#include "define.h"

using namespace std; 

#ifndef __SYMBOLTABLE__
#define __SYMBOLTABLE__
struct Type{
  string name;
  vector<int> cap;

  void print();
  void init();
  
  Type(string name = string(""), vector<int> cap = vector<int>()) : name(name), cap(cap) {}
};

struct Entry{
  string name;  // Max length 32
  string kind;
  Type type;
  vector<Type> attr;


  void printattr();
  void init();
  Entry(string name = "", string kind = "") : name(name), kind(kind) {}
};

struct Table{
  int level;
  set<string> valid;
  set<string> loopvalid;
  vector<Entry> entry;

  void add(Entry);
  void print();
  Table(){
    level = 0;
    valid.clear();
    loopvalid.clear();
    entry.clear();
  }
};

struct Symboltable{
  vector<Table> table;
  
  void add();
  Entry *find(string);
  void print();
};
#endif
