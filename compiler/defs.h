#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Structure and enum definitions
// Copyright (c) 2019 Warren Toomey, GPL3

// primitive types
enum {
  P_NONE, P_VOID, P_CHAR, P_INT, P_LONG,
  P_VOIDPTR, P_CHARPTR, P_INTPTR, P_LONGPTR
};

// Token types
enum {
  T_EOF, 
  // Operators
  T_PLUS, T_MINUS,
  T_STAR, T_SLASH,
  T_EQ, T_NE,
  T_LT, T_GT, T_LE, T_GE,
  // Type keywords
  T_VOID, T_CHAR, T_INT, T_LONG,
  // Structural tokens
  T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT,
  T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
  T_AMPER, T_LOGAND,
  // Other keywords
  T_PRINT, T_IF, T_ELSE, T_WHILE, T_FOR, T_RETURN
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

// AST node types. The first few line up
// with the related tokens
enum {
  A_ADD=1, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
  A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE,
  A_INTLIT,
  A_IDENT, A_LVIDENT, A_ASSIGN, A_PRINT, A_GLUE, 
  A_IF, A_WHILE, A_FUNCTION, A_WIDEN, A_RETURN,
  A_FUNCCALL, A_DEREF, A_ADDR
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;				            // "Operation" to be performed on this tree
  int type;			            // primitive type of the node
  struct ASTnode *left;			// Left and right child trees
  struct ASTnode *mid;			
  struct ASTnode *right;
  union {
    int intvalue;           // For A_INTLIT, the integer value
    int id;                 // For A_IDENT, the index into the symbol table
  } v;				
};

#define NOREG	-1		// Use NOREG when the AST generation
				// functions have no register to return

// Structural types
enum {
  S_VARIABLE, S_FUNCTION
};
        
// Symbol table structure
struct symtable {
  char *name;
  int type;			  // primitive type of the symbol
  int stype;      // structural type of the symbol
  int endlabel;   // label for the end of the function
};

#endif // DEFS_H