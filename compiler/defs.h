#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Structure and enum definitions
// Copyright (c) 2019 Warren Toomey, GPL3

// Token types
enum {
  T_EOF, T_INTLIT, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_SEMI, T_PRINT,
};

// Token structure
struct Token {
  int type;				    // Token type, from the enum list above
  int value;				  // For T_INTLIT, the integer value
};

struct CurChar
{
    char type;
    int processed;
};

// AST node types
enum {
   A_INTLIT, A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;				// "Operation" to be performed on this tree
  struct ASTnode *left;			// Left and right child trees
  struct ASTnode *right;
  int intvalue;				// For A_INTLIT, the integer value
};


// Symbol table structure
struct symtable {
  char *name
};

#endif