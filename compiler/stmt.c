#ifndef STMT_C
#define STMT_C

#include "defs.h"
#include "expressions.c"
#include "gen_asm.c"
#include "decl.c"
#include "misc.c"
#include "scan.c"

struct ASTnode* print_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree;
  int reg;

  // Match a 'print' as the first token
  match(file, curChar, token, T_PRINT, "print");

  // Parse the following expression and
  // generate the assembly code
  tree = binaryExpression(file, curChar, token, 0);

  tree = mkastnode(A_PRINT, tree, NULL, NULL, 0);

  // Match the following semicolon
  semi(file, curChar, token);
  return (tree);
}

struct ASTnode* assignment_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
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
  match(file, curChar, token, T_ASSIGN, "=");

  // Parse the following expression
  left = binaryExpression(file, curChar, token, 0);

  // Make an assignment AST tree
  tree = mkastnode(A_ASSIGN, left, NULL, right, 0);

  // Match the following semicolon
  semi(file, curChar, token);
  return (tree);
}

struct ASTnode* compound_statement(FILE *file, struct CurChar *curChar, struct Token *token);

// Parse an IF statement including
// any optional ELSE clause
// and return its AST
struct ASTnode *if_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *condAST, *trueAST, *falseAST = NULL;
  int reg;

  match(file, curChar, token, T_IF, "if");
  lparen(file, curChar, token);

  condAST = binaryExpression(file, curChar, token, 0);

  if (condAST->op < A_EQ || condAST->op > A_GE)
    fatal("Bad operator in if statement");
  rparen(file, curChar, token);

  trueAST = compound_statement(file, curChar, token);

  if (token->type == T_ELSE) {
    lexScan(file, curChar, token);
    falseAST = compound_statement(file, curChar, token);
  }
  
  return (mkastnode(A_IF, condAST, trueAST, falseAST, 0));
}

struct ASTnode* while_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *condAST, *bodyAST;

  match(file, curChar, token, T_WHILE, "while");
  lparen(file, curChar, token);

  // find condition expression, and check if correct grammer is used
  condAST = binaryExpression(file, curChar, token, 0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
    fatal("Bad operator in if statement");
  rparen(file, curChar, token);

  bodyAST = compound_statement(file, curChar, token);
  return (mkastnode(A_WHILE, condAST, NULL, bodyAST, 0));
}

struct ASTnode* compound_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *left = NULL;
  struct ASTnode *tree;
  int reg;

  lbrace(file, curChar, token);

  while (1) {
    switch (token->type) {
      case T_PRINT:
        tree = print_statement(file, curChar, token);
        printf("found print statement\n");
        break;
      case T_INT:
        var_declaration(file, curChar, token);
        printf("found variable declaration\n");
        tree = NULL;
        break;
      case T_IDENT:
        tree = assignment_statement(file, curChar, token);
        printf("found assignment statement\n");
        break;
      case T_IF:
        tree = if_statement(file, curChar, token);
        printf("found if statement\n");
        break;
      case T_WHILE:
        tree = while_statement(file, curChar, token);
        printf("found while statement\n");
        break;
      case T_RBRACE:
        rbrace(file, curChar, token);
        return (left);

      default:
        fatald("Syntax error, token", token->type);
    }

    if (tree) {
      if (left == NULL) {
        left = tree;
      } else {
        left = mkastnode(A_GLUE, left, NULL, tree, 0);
      }
    }
  }
}

#endif // STMT_C