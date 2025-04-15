#include "defs.h"
#include "expressions.c"

// Parse one or more statements
void statements(FILE *file, struct CurChar *curChar, struct Token *token) {
    struct ASTnode *tree;
    int reg;
  
    while (1) {
      // Match a 'print' as the first token
      match(file, curChar, token, T_PRINT, "print");
  
      // Parse the following expression and
      // generate the assembly code
      tree = binaryExpression(file, curChar, token, 0);
      reg = genAST(tree);
      cgprintint(reg);
      freeall_registers();
  
      // Match the following semicolon
      // and stop if we are at EOF
      semi(file, curChar, token);
      printf("Value calculated: %d\n", intepretASTTree(tree));

      if (token->type == T_EOF)
        return;
    }
  }