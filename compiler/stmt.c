#ifndef STMT_C
#define STMT_C

#include "defs.h"
#include "expressions.c"
#include "gen_asm.c"
#include "decl.c"

void print_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree;
  int reg;

  // Match a 'print' as the first token
  match(file, curChar, token, T_PRINT, "print");

  // Parse the following expression and
  // generate the assembly code
  tree = binaryExpression(file, curChar, token, 0);
  reg = genAST(tree, -1);
  cgprintint(reg);
  freeall_registers();

  // Match the following semicolon
  semi(file, curChar, token);
}

void assignment_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *left, *right, *tree;
  int id;

  // Ensure we have an identifier
  ident(file, curChar, token);

  // Check it's been defined then make a leaf node for it
  if ((id = findglob(Text)) == -1) {
    printf("Undeclared variable %s", Text);
  }
  right = mkastleaf(A_LVIDENT, id);

  // Ensure we have an equals sign
  match(file, curChar, token, T_EQUALS, "=");

  // Parse the following expression
  left = binaryExpression(file, curChar, token, 0);

  // Make an assignment AST tree
  tree = mkastnode(A_ASSIGN, left, right, 0);

  // Generate the assembly code for the assignment
  genAST(tree, -1);
  freeall_registers();

  // Match the following semicolon
  semi(file, curChar, token);
}


// Parse one or more statements
void statements(FILE *file, struct CurChar *curChar, struct Token *token) {
    struct ASTnode *tree;
    int reg;
  
    while (1) {
      switch (token->type) {
        case T_PRINT:
          print_statement(file, curChar, token);
          break;
        case T_INT:
          var_declaration(file, curChar, token);
          break;
        case T_IDENT:
          assignment_statement(file, curChar, token);
          break;
        case T_EOF:
          return;
        default:
          printf("Error: unexpected token in statements\n");
          exit(1);
      }
  }
}

#endif // STMT_C