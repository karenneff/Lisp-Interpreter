/*
Name: Karen Neff
Class: CS 3160
Due Date: 12-5-2015
Assignment: Homework 13, open-ended project
Files: lispinterpreter.cpp, lispparser.h, lispobjects.h, datastructures.h
Description: This file is a parser for taking apart input strings
and translating them to the corresponding LispExpression objects.
*/

#include <iostream>
#include <string>
#include "lispobjects.h"
#include "datastructures.h"

using namespace std;

float toFloat (string s);

LispExpression* parseWithStack(string input, LinkedStack<LispExpression>* parenExp, bool literal,varTable<LispExpression> *table)
{
   LinkedStack<LispExpression> operExp; 
   size_t first = input.find_last_of("(");//find the last open paren
   size_t previous = first;
   while(first!= -1)//Expression contains one or more sub-expressions in parentheses; parse these first
   {
      string remainder = input.substr(first);
      if(remainder.find_first_of(")") == -1)//find the matching close paren
         throw "Unpaired open parenthesis!";
      size_t last = remainder.find_first_of(")");
      int length = last-1;//determine the length of the enclosed expression
      char c = 0; //find character before parentheses, if any
      if(first!=0)
         c = input.at(first-1);
      if(c==39)//single quote before parentheses; treat as list
      {
         if(length==0)
         {
            parenExp->push(new ListType());//nothing in parentheses, but treated as literal, i.e. empty list
            input.replace(first-1, 3, " @ "); //replace with a marker character, so parser can find it later
         }
         else
         { 
            parenExp->push(parseWithStack(remainder.substr(1,length),parenExp,true,table));//parse what was in parentheses as a list
            input.replace(first-1, length+3, " @ "); //replace with a marker character
         }
      }
      else if(length!=0)//not a literal; parse contents as an ordinary expression
      {
         parenExp->push(parseWithStack(remainder.substr(1,length),parenExp,false,table));//parse what was in parentheses; stack parenExp must be included so parser can access nested expressions
         input.replace(first, length+2, " @ "); //replace with a marker character, so parser can find it later
      }
      else
         input.replace(first, 2, " "); //too many parentheses; simply delete them
      previous = first;
      first = input.find_last_of("(");//find the last open paren
   }
   
   //now there are no more sub-expressions; parse each symbol individually
   if(input.find_last_of(")") != -1)
      throw "Unpaired close parentheses!";
   LinkedStack<LispExpression> localParen; //locates and stores any sub-expressions that were in parentheses; must be stored separately so as not to reverse the order.
   for(int i = 0; i<input.length();i++)
   {
      if(input.at(i)=='@')
      {
         if(parenExp->getSize()==0)
            throw "@ cannot be used as a variable name!";
         localParen.push(parenExp->pop());
      }
   }
   size_t whitespace;
   do{
      whitespace = input.find_last_of(" ");
      if(whitespace==input.length()-1)//expression ends with a space; erase it before trying to parse
         input.erase(whitespace);
      else//expression ends with a non-whitespace character
      {
         string symbol = input.substr(whitespace+1);//find last number, symbol, etc. in expression to be parsed
         if(symbol=="+"||symbol=="-"||symbol=="*"||symbol=="/")//arithmetic expression
            operExp.push(new ArithExp(symbol.at(0),&operExp));
         else if(symbol.at(0)<58&&symbol.at(0)>47||symbol.at(0)=='-')//digit or minus sign
            operExp.push(new Number(toFloat(symbol)));//parse as a number
         else if(symbol=="@")//parentheses marker symbol
         { 
            operExp.push(localParen.pop());//retrieve expression in parentheses that was parsed earlier
         }
         else if(symbol=="<"||symbol==">"||symbol=="<="||symbol==">=")//comparison expression
         {
            symbol = symbol + " ";//ensure symbol has at least two characters, to make reading easier later
            bool equals=false;
            if(symbol.at(1)=='=')
               equals=true;
            operExp.push(new CompExp(symbol.at(0),equals,&operExp));
         }
         else if(symbol=="and")//logic expression
            operExp.push(new LogicExp(true,&operExp));
         else if(symbol=="or")//logic expression
            operExp.push(new LogicExp(false,&operExp));
         else if(symbol=="not")//not expression
            operExp.push(new NotExp(&operExp));
         else if(symbol=="cons")
            operExp.push(new ConsExp(&operExp));
         else if(symbol=="car")
            operExp.push(new ListIterExp('a',&operExp));
         else if(symbol=="cdr")
            operExp.push(new ListIterExp('d',&operExp));
         else if(symbol=="define")
            operExp.push(new DefineExp(&operExp,table));
         else if(symbol.at(0)==39)//single quote; signals symbol
            operExp.push(new Symbol(symbol.substr(1)));
         else if(literal)//symbol is one of several list elements
            operExp.push(new Symbol(symbol));
         else //no match found; treat as a variable
            operExp.push(new Variable(symbol,table));
         input.erase(whitespace+1);//finally, erase the number, symbol, etc that was just parsed
      }
   }while(whitespace != -1 && input != "");
   
   if(literal) //expression was to be parsed as a list; retrieve and append everything on operand stack, return resulting list
   {
      ListType *l = new ListType();
      while(operExp.getSize()>0)
         l->append(operExp.pop());
      return l;
   }
   if(operExp.getSize()==0) 
      throw "No valid expression entered!";
   LispExpression *result = operExp.pop();
   if(operExp.getSize()>0)
      throw "Can't evaluate- more than one expression entered!";
   return result;
}

LispExpression* parse(string input, varTable<LispExpression> *t)
{
   LinkedStack<LispExpression> p;
   LispExpression* result = parseWithStack(input,&p,false,t);
   if(p.getSize()>0)
      throw "Too many operands!";
   return result;
   
}

float toFloat (string s)
{
   if(s.at(0)=='-')
      s = s.substr(1) + "-";//place sign at the end before parsing
   else
      s = s + "+";
   float result = 0.0;
   while(s.at(0)<58&&s.at(0)>47)//parse all digits before the decimal point
   {
      int next = s.at(0)-48;
      result *= 10;
      result += next;
      s = s.substr(1);
   }
   if(s.at(0)=='.')//decimal point
   {
      s = s.substr(1);
      int exponent = 1;
      while(s.at(0)<58&&s.at(0)>47)//parse digits after decimal point
      {
         float next = s.at(0)-48;
         for(int i = 1; i <= exponent; i++)
            next = next/10;
         result += next;
         s = s.substr(1);
         exponent++;
      }
   }
   if(s.at(0)=='-')//marker was reached; parsing complete
      result = result * -1;
   else if(s.at(0)!='+')
      throw "Improper number format!"; //letter, extra decimal point, etc. was encountered during parsing
   return result;
}