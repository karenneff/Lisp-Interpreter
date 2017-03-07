/*
Name: Karen Neff
Class: CS 3160
Due Date: 12-5-2015
Assignment: Homework 13, open-ended project
Files: lispinterpreter.cpp, lispparser.h, lispobjects.h, datastructures.h
Description: This file is the driver file for the lisp
interpreter.
*/

#include <iostream>
#include "lispobjects.h"
#include "lispparser.h"

using namespace std;

int main()
{
   cout << "Enter a lisp expression here, or press enter to exit." <<endl;
   string entry;
   varTable<LispExpression> table;
   table.add("#true",new Boolean(true));
   table.add("#false",new Boolean(false));
   do{ //read in a line at a time, send to parser if non-empty
      getline(cin, entry);
      if(entry.length()==0)
         cout << "Goodbye!";
      else
         try{
            LispExpression *result = parse(entry, &table);
            result = result->evaluate();
            result->print();
         }
         catch(const char* s)
         {
            cout << s <<endl;
         }
      cout << endl;
   }while(entry.length() > 0);
}