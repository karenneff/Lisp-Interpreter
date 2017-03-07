/*
Name: Karen Neff
Class: CS 3160
Due Date: 12-5-2015
Assignment: Homework 13, open-ended project
Files: lispinterpreter.cpp, lispparser.h, lispobjects.h, datastructures.h
Description: This file defines a stack object that holds
LispExpression objects, to be used while parsing, and a vartable
object to store variable names ant the associated objects.
*/

#ifndef _DATASTRUCTURES_H
#define _DATASTRUCTURES_H

#include <string>

using namespace std;

template <class Type> 
struct StackElement //can be used to build stacks or lists of objects
{
   Type *value;
   string name;
   StackElement<Type> *link;
};

template <class Type>
class LinkedStack //basic stack
{
   private:
      StackElement<Type> *top;
      int size;
   public:
      LinkedStack()
      {
         top = NULL;
         size = 0;
      }
      
      int getSize()
      {
         return size;
      }
      
      void push(Type *next)
      {
         StackElement<Type> *s;
         s = new StackElement<Type>;
         s->link = top;
         s->value = next;
         top = s;
         size++;
      }
      
      Type* pop()
      {
         if(size==0)
            throw "Too few objects on stack!";
         Type *result = top->value;
         top = top->link;
         size--;
         return result;
      }
};

template <class Type>
class varTable //variable lookup table; stores objects with associated names
{
   private:
      StackElement<Type> *top;
   public:
      varTable()
      {
         top = NULL;
      }
      void add(string x, Type* y) //takes an object and associated name, adds these to the table
      {
         StackElement<Type> *s;
         s = new StackElement<Type>;
         s->name = x;
         s->value = y;
         s->link = top;
         top = s;
      }
      Type* search(string varName) //if the variable has been defined, returns an object representing its value; returns null otherwise
      {
         StackElement<Type> *i;
         i = top;
         while(i != NULL)
         {
            if(i->name == varName)
               return i->value;
            i = i->link;
         }
         return NULL;
      }
};

#endif