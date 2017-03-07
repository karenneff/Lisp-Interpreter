/*
Name: Karen Neff
Class: CS 3160
Due Date: 12-5-2015
Assignment: Homework 13, open-ended project
Files: lispinterpreter.cpp, lispparser.h, lispobjects.h, datastructures.h
Description: This file contains objects representing various
expression and data types found in Lisp.
*/

#ifndef _LISPOBJECTS_H
#define _LISPOBJECTS_H

#include <iostream>
#include "datastructures.h"

using namespace std;

class LispExpression 
{ 
protected:
   char type; //s=symbol, n=number, b=boolean, l=list, f=function, v=variable, z=null
   LispExpression *link; //for use in building lists
public:
   LispExpression()
   {
      link = NULL;
   }
   char getType() 
   {
      return type;
   }
   LispExpression* getLink()
   {
      return link;
   }
   void setLink(LispExpression *l)
   {
      link = l;
   }
   virtual LispExpression* evaluate() = 0; //returns pointer to an object which is the result of computation, etc, or returns itself if an atom
   virtual void print() = 0; //used to print the final result of computation
   virtual LispExpression* copy() = 0; //return an exact copy; used for retrieving values of variables
};

class Number: public LispExpression //atomic number value
{
private:
   float value;
public:
   Number()
   {
      type = 'n';
      value = 0;
   }
   Number(float x)
   {
      type = 'n';
      value = x;
   }
   Number* evaluate()
   {
      return this;
   }
   float getValue()
   {
      return value;
   }
   void print()
   {
      cout << value << " - type: number" <<endl;
   }
   Number* copy()
   {
      return new Number(value);
   }
};

class ArithExp: public LispExpression //expressions that operate on numbers and return numbers
{
private:
   char oper;
   LispExpression *opd1, *opd2;
public:
   ArithExp(char c, LispExpression *x, LispExpression *y)
   {
      type = 'n';
      oper = c;
      opd1 = x;
      opd2 = y;
   }
   ArithExp(char c, LinkedStack<LispExpression> *s)
   {
      type = 'n';
      oper = c;
      try{
         opd1 = s->pop();
         opd2 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   Number* evaluate()
   {
      opd1 = opd1->evaluate();
      opd2 = opd2->evaluate();
      if(opd1->getType()!='n'||opd2->getType()!='n')
         throw "Type error: arithmetic expressions require numbers!";
      Number *n1 = (Number*)opd1;
      Number *n2 = (Number*)opd2;
      float x = n1->getValue();
      float y = n2->getValue();
      switch(oper)
      {
         case '+': return new Number(x + y);
         case '-': return new Number(x - y);
         case '*': return new Number(x * y);
         case '/': return new Number(x / y); 
      }
   }
   void print()
   {
      cout << oper << " expression - return type: number" <<endl;
   }
   ArithExp* copy()
   {
      return new ArithExp(oper, opd1->copy(), opd2->copy());
   }
};

class Symbol: public LispExpression //atomic symbol data type
{
private:
   string name;
public:
   Symbol()
   {
      type = 's';
      name = "";
   }
   Symbol(string s)
   {
      type = 's';
      name = s;
   }
   Symbol* evaluate()
   {
      return this;
   }
   void print()
   {
      cout << name << " - type:symbol" << endl;
   }
   Symbol* copy()
   {
      return new Symbol(name);
   }
   
};

class Variable: public LispExpression //variable created using define, or mistakenly by not using ' at the start of a symbol
{
private:
   string name;
   varTable<LispExpression> *table;
public:
   Variable(string s, varTable<LispExpression> *t)
   {
      type = 'v';
      name = s;
      table = t;
   }
   string getName()
   {
      return name;
   }
   LispExpression* evaluate()
   {
      LispExpression *l = table->search(name);
      if(l == NULL)
         throw "Undefined variable!";
      l = l->copy();
      return l->evaluate();
      
   }
   void print()
   {
      cout << name << " - type:variable" << endl;
   }
   Variable* copy()
   {
      return new Variable(name, table);
   }
   
};

class Boolean: public LispExpression //atomic boolean value
{
private:
   bool value;
public:
   Boolean()
   {
      type = 'b';
      value = false;
   }
   Boolean(bool x)
   {
      type = 'b';
      value = x;
   }
   Boolean* evaluate()
   {
      return this;
   }
   bool getValue()
   {
      return value;
   }
   void print()
   {
      if(value)
         cout << "true";
      else
         cout << "false";
      cout << " - type: boolean" <<endl;
   }
   Boolean* copy()
   {
      return new Boolean(value);
   }
};

class CompExp: public LispExpression //includes <=, >=, etc - takes two numbers, returns boolean
{
private:
   char oper; // < or >
   bool eqOK; //true for >=, false for >
   LispExpression *opd1, *opd2;
public:
   CompExp(char c, bool b, LispExpression *x, LispExpression *y)
   {
      type = 'b';
      oper = c;
      eqOK = b;
      opd1 = x;
      opd2 = y;
   }
   CompExp(char c, bool b, LinkedStack<LispExpression> *s)
   {
      type = 'b';
      oper = c;
      eqOK = b;
      try{
         opd1 = s->pop();
         opd2 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   Boolean* evaluate()
   {
      opd1 = opd1->evaluate();
      opd2 = opd2->evaluate();
      if(opd1->getType()!='n'||opd2->getType()!='n')
         throw "Type error: comparison expressions require numbers!";
      Number *n1 = (Number*)opd1;
      Number *n2 = (Number*)opd2;
      float x,y;
      if(oper=='<')
      {
         x = n1->getValue();
         y = n2->getValue();
      }
      else if(oper=='>')
      {
         y = n1->getValue();
         x = n2->getValue();
      }
      bool finalValue = false;
      if(x<y || (eqOK && x==y))
         finalValue=true;
      return new Boolean(finalValue);
   }
   void print()
   {
      cout << oper; 
      if(eqOK)
         cout << '=';
      cout << " expression - return type: boolean" <<endl;
   }
   CompExp* copy()
   {
      return new CompExp(oper, eqOK, opd1->copy(),opd2->copy());
   }
};

class LogicExp: public LispExpression //includes and, or - takes two booleans, returns boolean
{
private:
   bool all; //true for and, false for or
   LispExpression *opd1, *opd2;
public:
   LogicExp(bool b, LispExpression *x, LispExpression *y)
   {
      type = 'b';
      all = b;
      opd1 = x;
      opd2 = y;
   }
   LogicExp(bool b, LinkedStack<LispExpression> *s)
   {
      type = 'b';
      all = b;
      try{
         opd1 = s->pop();
         opd2 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   Boolean* evaluate()
   {
      opd1 = opd1->evaluate();
      opd2 = opd2->evaluate();
      if(opd1->getType()!='b'||opd2->getType()!='b')
         throw "Type error: logic expressions require booleans!";
      Boolean *n1 = (Boolean*)opd1;
      Boolean *n2 = (Boolean*)opd2;
      bool x,y;
      x = n1->getValue();
      y = n2->getValue();
      if(all && x)
         x = y;
      else if ((! all) && y)
         x = true;
      return new Boolean(x);
   }
   void print()
   {
      if(all)
         cout << "and";
      else
         cout << "or";
      cout << " expression - return type: boolean" <<endl;
   }
   LogicExp* copy()
   {
      return new LogicExp(all,opd1->copy(),opd2->copy());
   }
};

class NotExp: public LispExpression //takes one boolean, returns the opposite
{
private:
   LispExpression *opd1;
public:
   NotExp(LispExpression *x)
   {
      type = 'b';
      opd1 = x;
   }
   NotExp(LinkedStack<LispExpression> *s)
   {
      type = 'b';
      try{
         opd1 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   Boolean* evaluate()
   {
      if(opd1->getType()!='b')
         throw "Type error: logic expressions require booleans!";
      opd1 = opd1->evaluate();
      Boolean *n1 = (Boolean*)opd1;
      bool finalValue = !(n1->getValue());
      return new Boolean(finalValue);
   }
   void print()
   {
      cout << "not expression - return type: boolean" <<endl;
   }
   NotExp* copy()
   {
      return new NotExp(opd1->copy());
   }
};

class ListType: public LispExpression //list data structure; can hold all types of expressions
{
private:
   LispExpression *nextOnList;
public:
   ListType() //creates an empty list
   {
      type = 'l';
      link = NULL;
      nextOnList = NULL;
   }
   ListType* cons(LispExpression *next)
   {
      ListType* l = new ListType();
      l->append(next);
      LispExpression *i = nextOnList;
      while(i != NULL)
      {
         l->append(i->copy());
         i = i->getLink();
      }
      return l;
   }
   LispExpression* car()
   {
      if(nextOnList == NULL)
         throw "Can\'t car an empty list!";
      return nextOnList->copy();
   }
   ListType* cdr()
   {
      ListType* l = new ListType();
      LispExpression *i = nextOnList;
      if(i==NULL)
         throw "Can\'t cdr an empty list!";
      i = i->getLink();//skip first element
      while(i != NULL)
      {
         l->append(i->copy());
         i = i->getLink();
      }
      return l;
   }
   ListType* append(LispExpression *next) //used when constructing lists from literals typed by user, and when copying
   {
      if(nextOnList==NULL)
         nextOnList=next;
      else
      {  
         LispExpression *i = nextOnList;
         while(i->getLink() != NULL)
         {
            i = i->getLink();
         }
         i->setLink(next);
      }
      return this;
   }
   ListType* evaluate()
   {
      return this;
   }
   void print()
   {
      if(nextOnList==NULL)
         cout << "Empty list" << endl;
      else
      {
         LispExpression *i = nextOnList;
         cout << "List containing: " <<endl;
         while(i != NULL)
         {
            i->print();
            i = i->getLink();
         
         }
      }
   }
   ListType* copy()
   {
      ListType* l = new ListType();
      LispExpression *i = nextOnList;
      while(i != NULL)
      {
         l->append(i->copy());
         i = i->getLink();
      }
      return l;
   }
   
};

class ConsExp: public LispExpression //takes an element and a list, returns a new list
{
private:
   LispExpression *opd1, *opd2;
public:
   ConsExp(LispExpression *x, LispExpression *y)
   {
      type = 'l';
      opd1 = x;
      opd2 = y;
   }
   ConsExp(LinkedStack<LispExpression> *s)
   {
      type = 'l';
      try{
         opd1 = s->pop();
         opd2 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   ListType* evaluate()
   {
      opd1 = opd1->evaluate();
      opd2 = opd2->evaluate();
      if(opd2->getType()!='l')
         throw "Type error: list operation requires a list!";//opd1 can be any type
      ListType *n1 = (ListType*)opd2;
      return n1->cons(opd1);
   }
   void print()
   {
      cout << "cons expression - return type: list" <<endl;
   }
   ConsExp* copy()
   {
      return new ConsExp(opd1->copy(),opd2->copy());
   }
};

class ListIterExp: public LispExpression //includes car and cdr
{
private:
   char oper;
   LispExpression *opd1;
public:
   ListIterExp(char c, LispExpression *x)
   {
      type = 'l';
      oper = c;
      opd1 = x;
   }
   ListIterExp(char c, LinkedStack<LispExpression> *s)
   {
      type = 'l';
      oper = c;
      try{
         opd1 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   LispExpression* evaluate()
   {
      opd1 = opd1->evaluate();
      if(opd1->getType()!='l')
         throw "Type error: list operation requires a list!";//opd1 can be any type
      ListType *n1 = (ListType*)opd1;
      if(oper=='a')
         return n1->car();
      else if(oper=='d')
         return n1->cdr();
   }
   void print()
   {
      cout << "c" <<oper << "r expression - return type: list" <<endl;
   }
   ListIterExp* copy()
   {
      return new ListIterExp(oper,opd1->copy());
   }
};

class DefineExp: public LispExpression //includes <=, >=, etc - takes two numbers, returns boolean
{
private:
   LispExpression *opd1, *opd2;
   varTable<LispExpression> *table;
public:
   DefineExp(LispExpression *x, LispExpression *y, varTable<LispExpression> *t)
   {
      type = 'z';
      opd1 = x;
      opd2 = y;
      table = t;
   }
   DefineExp(LinkedStack<LispExpression> *s, varTable<LispExpression> *t)
   {
      type = 'z';
      table = t;
      try{
         opd1 = s->pop();
         opd2 = s->pop();
      }
      catch(const char* x)//occurs if stack does not have enough operands
      {
         throw "Too few operands!";
      }
      if(s->getSize()!=0)
         throw "Too many operands!";
   }
   DefineExp* evaluate()
   {
      if(opd1->getType()!='v')
         throw "Invalid variable name!";
      Variable *v = (Variable*)opd1;
      if(table->search(v->getName()) != NULL)
         throw "Can\'t redefine a variable!";
      table->add(v->getName(),opd2);
      return this;
   }
   void print()
   {
      cout << "New variable defined." <<endl;
   }
   DefineExp* copy()
   {
      return new DefineExp(opd1->copy(),opd2->copy(),table);
   }
};

#endif