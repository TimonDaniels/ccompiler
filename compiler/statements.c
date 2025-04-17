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

// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree;
  int nameslot;

  // Find the 'void', the identifier, and the '(' ')'.
  // For now, do nothing with them
  match(file, curChar, token, T_VOID, "void");
  ident(file, curChar, token);
  nameslot= addglob(Text);
  lparen(file, curChar, token);
  rparen(file, curChar, token);

  // Get the AST tree for the compound statement
  tree = compound_statement(file, curChar, token);

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (mkastnode(A_FUNCTION, tree, NULL, NULL, nameslot));
}

struct ASTnode* single_statement(FILE *file, struct CurChar *curChar, struct Token *token);

// Parse a FOR statement
// and return its AST
static struct ASTnode *for_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *condAST, *bodyAST;
  struct ASTnode *preopAST, *postopAST;
  struct ASTnode *tree;

  // Ensure we have 'for' '('
  match(file, curChar, token, T_FOR, "for");
  lparen(file, curChar, token);

  // Get the pre_op statement and the ';'
  preopAST= single_statement(file, curChar, token);
  semi(file, curChar, token);

  // Get the condition and the ';'
  condAST = binaryExpression(file, curChar, token, 0);
  if (condAST->op < A_EQ || condAST->op > A_GE)
    fatal("Bad comparison operator");
  semi(file, curChar, token);

  // Get the post_op statement and the ')'
  postopAST= single_statement(file, curChar, token);
  rparen(file, curChar, token);

  // Get the compound statement which is the body
  bodyAST = compound_statement(file, curChar, token);

  // For now, all four sub-trees have to be non-NULL.
  // Later on, we'll change the semantics for when some are missing

  // Glue the compound statement and the postop tree
  tree= mkastnode(A_GLUE, bodyAST, NULL, postopAST, 0);

  // Make a WHILE loop with the condition and this new body
  tree= mkastnode(A_WHILE, condAST, NULL, tree, 0);

  // And glue the preop tree to the A_WHILE tree
  return(mkastnode(A_GLUE, preopAST, NULL, tree, 0));
}

struct ASTnode* single_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree = NULL;

  switch (token->type) {
    case T_PRINT:
      printf("found print statement\n");
      return (print_statement(file, curChar, token));
    case T_INT:
      printf("found variable declaration\n");
      var_declaration(file, curChar, token);
      return (NULL);
    case T_IDENT:
      printf("found assignment statement\n");
      return (assignment_statement(file, curChar, token));
    case T_IF:
      printf("found if statement\n");
      return (if_statement(file, curChar, token));
    case T_WHILE:
      printf("found while statement\n");
      return (while_statement(file, curChar, token));
    case T_FOR:
      printf("found for statement\n");
      return (for_statement(file, curChar, token));

    default:
      fatald("Syntax error, token", token->type);
  }
  return (tree);
}

struct ASTnode* compound_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *left = NULL;
  struct ASTnode *tree;
  int reg;

  lbrace(file, curChar, token);

  while (1) {

    tree = single_statement(file, curChar, token);

    if (tree != NULL && (tree->op == A_PRINT || tree->op == A_ASSIGN)) {
      semi(file, curChar, token);
    }

    if (tree) {
      if (left == NULL)
        left = tree;
      else
        left = mkastnode(A_GLUE, left, NULL, tree, 0);
    }

    if (token->type == T_RBRACE) {
      rbrace(file, curChar, token);
      return (left);
    }
  }
}

#endif // STMT_C