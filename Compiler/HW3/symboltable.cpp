#include "symboltable.h"
#include <cstring>

void Symboltable::add(){
  Table tmp;
  if(!this->table.empty()) tmp.loopvalid = this->table.top().loopvalid;
  tmp.level = table.size();
  this->table.push(tmp);
}

void Symboltable::print(){
  extern int Opt_D;
  if (Opt_D) {
    for(int i = 0; i < 110; i++) printf("=");
    printf("\n");
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
    for(int i = 0; i < 110; i++) printf("-");
    printf("\n");
    for(auto tt : this->table.top().entry){
      printf("%-33s%-11s", tt.name.c_str(), tt.kind.c_str());
      this->table.top().print();
      tt.type.print();
      tt.printattr();
    }
    for(int i = 0; i < 110; i++) printf("-");
    printf("\n");
  }
  
  this->table.pop();
}

void Table::add(Entry tmp){
  if(this->valid.find(tmp.name) != this->valid.end()){
    extern int linenum;
    string errmsg = "symbol " + tmp.name + " is redeclared\n";
    cout << "<Error> found in Line " << linenum << ": " << errmsg; 
    return;
  }

  this->valid.insert(tmp.name);
  entry.push_back(tmp);
  this->entry.back().type = tmp.type;
  for(int i = 0; i < int(tmp.attr.size()); i++)
    this->entry.back().attr[i] = Type(tmp.attr[i]);
}

void Table::print(){
  if(this->level == 0){
    printf("%d%-10s", 0, "(global)"); 
  }
  else{
    printf("%d%-10s", this->level, "(local)");
  }
}

void Entry::printattr(){
  string tmp;
  int flag = 0;
  for(auto obj : this->attr){
    if(flag){
      tmp += ", ";
      flag = 0;
    }
    if(obj.name != ""){
      tmp += obj.name;
      if(obj.cap.size() > 0) tmp += " ";
      for(auto i : obj.cap) tmp += "[" + to_string(i) + "]";
    }
    else{
      flag = 0;
      continue;
    }
    flag = 1;
  }
  printf("%-11s\n", tmp.c_str());
}

void Entry::init(){
  this->name.clear();
  this->kind.clear();
  this->type.init();
  this->attr.clear();
}

void Type::print(){
  string tmp = this->name + " ";
  for(auto i : this->cap){
    stringstream ss;
    string qq;
    ss << "[" << i << "]";
    ss >> qq;
    tmp += qq;
  }
  printf("%-17s", tmp.c_str());
}

void Type::init(){
  this->name.clear();
  this->cap.clear();
}

void mystrcat(char *target, char *add){
  char tmp[1005];
  strcpy(tmp, add);
  strcat(tmp, " "); 
  strcat(tmp, target);
  strcpy(target, tmp);
}
