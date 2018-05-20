#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <stack>
#include <vector>
#include <set>

using namespace std; 

struct Type{
  string name;
  vector<int> cap;

  void print();
  void init();
  
  Type(){}
  Type(string name) : name(name) {}
  Type(const Type &type){
    this->name = type.name;
    this->cap = type.cap;
  }
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
};

struct Symboltable{
  stack<Table> table;
  
 void add();
 void print();
};
