#ifndef STATEMENTS_C

#define STATEMENTS_C


#include "defs.h"
#include "expressions.c"
#include "gen_asm.c"
#include "decl.c"
#include "misc.c"
#include "scan.c"

struct ASTnode* print_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree;
  int lefttype, righttype;
  int reg;

  // Match a 'print' as the first token
  match(file, curChar, token, T_PRINT, "print");

  // Parse the following expression and
  // generate the assembly code
  tree = binaryExpression(file, curChar, token, 0);

  lefttype = P_INT;
  righttype = tree->type;
  if (!type_compatible(&lefttype, &righttype, 0))
    fatal("Incompatible types in print statement");

  if (righttype)
    tree = mkastnode(righttype, P_NONE, tree, NULL, NULL, 0);

  tree = mkastnode(A_PRINT, P_NONE, tree, NULL, NULL, 0);

  return (tree);
}

struct ASTnode* assignment_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *left, *right, *tree;
  int lefttype, righttype;
  int id;

  // Ensure we have an identifier
  ident(file, curChar, token);

  if (token->type == T_LPAREN)
    return (funccall(file, curChar, token));

  // Check it's been defined then make a leaf node for it
  if ((id = findglob(Text)) == -1) {
    printf("Undeclared variable %s", Text);
  }
  right = mkastleaf(A_LVIDENT, Gsym[id].type, id);

  // Ensure we have an equals sign
  match(file, curChar, token, T_ASSIGN, "=");

  // Parse the following expression
  left = binaryExpression(file, curChar, token, 0);

  // Ensure the two types are compatible.
  lefttype = left->type;
  righttype = right->type;
  if (!type_compatible(&lefttype, &righttype, 1))
    fatal("Incompatible types");
  if (lefttype)
    left = mkastnode(lefttype, right->type, left, NULL, NULL, 0);

  // Make an assignment AST tree
  tree = mkastnode(A_ASSIGN, P_INT, left, NULL, right, 0);

  return (tree);
}

struct ASTnode* return_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode* tree;
  int function_type, return_type;

  if (Gsym[Functionid].type == P_VOID){
    fatal("Cannot return from a void function");
  }

  match(file, curChar, token, T_RETURN, "return");
  lparen(file, curChar, token);

  tree = binaryExpression(file, curChar, token, 0);

  return_type = tree->type;
  function_type = Gsym[Functionid].type;
  if (!type_compatible(&return_type, &function_type, 1))
    fatal("Incompatible types");

  // if type_compatible did set the left tree to A_WIDEN, we need to make a A_WIDEN node
  if (return_type);
    tree = mkastnode(return_type, function_type, tree, NULL, NULL, 0);

  // return a A_RETURN node
  tree = mkastnode(A_RETURN, P_NONE, tree, NULL, NULL, 0);

  rparen(file, curChar, token);
  return (tree);
}

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
  
  return (mkastnode(A_IF, P_NONE, condAST, trueAST, falseAST, 0));
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
  return (mkastnode(A_WHILE, P_NONE, condAST, NULL, bodyAST, 0));
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
  tree= mkastnode(A_GLUE, P_NONE, bodyAST, NULL, postopAST, 0);

  // Make a WHILE loop with the condition and this new body
  tree= mkastnode(A_WHILE, P_NONE, condAST, NULL, tree, 0);

  // And glue the preop tree to the A_WHILE tree
  return(mkastnode(A_GLUE, P_NONE, preopAST, NULL, tree, 0));
}

struct ASTnode* single_statement(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree = NULL;

  switch (token->type) {
    case T_PRINT:
      printf("found print statement\n");
      return (print_statement(file, curChar, token));
    case T_CHAR:
    case T_INT:
    case T_LONG:
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
    case T_RETURN:
      printf("found return statement\n");
      return (return_statement(file, curChar, token));
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

    if (tree != NULL && (tree->op == A_PRINT || tree->op == A_ASSIGN || 
      tree->op == A_RETURN || tree->op == A_FUNCCALL)) {
      semi(file, curChar, token);
    }

    if (tree) {
      if (left == NULL)
        left = tree;
      else
        left = mkastnode(A_GLUE, P_NONE, left, NULL, tree, 0);
    }

    if (token->type == T_RBRACE) {
      rbrace(file, curChar, token);
      return (left);
    }
  }
}

#endif // STATEMENTS_C
