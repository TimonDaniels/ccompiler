#include "global.h"
#include "defs.h"
#include "scan.c"
#include "symbols.c"

int parse_type(int t) {
  switch (t) {
    case T_INT:
      return (P_INT);
    case T_CHAR:
      return (P_CHAR);
    case T_VOID:
      return (P_VOID);
    case T_LONG:
      return (P_LONG);
    default:
      fatald("Invalid type", t);
  }
}

// Parse the declaration of a variable
void var_declaration(FILE *file, struct CurChar *curChar, struct Token *token) {
    int type, id;

    // Ensure we have an 'int' token followed by an identifier
    // and a semicolon. Text now has the identifier's name.
    // Add it as a known identifier
    type = parse_type(token->type);
    lexScan(file, curChar, token);
    ident(file, curChar, token);
    id = addglob(Text, type, S_VARIABLE, 0);
    cgglobsym(id);
    semi(file, curChar, token);
}

struct ASTnode* compound_statement(FILE *file, struct CurChar *curChar, struct Token *token);

// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree, *finalstmt;
  int nameslot, type, endlabel;

  type = parse_type(token->type);
  lexScan(file, curChar, token);
  ident(file, curChar, token);

  // Find the 'void', the identifier, and the '(' ')'.
  // For now, do nothing with them
  endlabel = label();
  nameslot = addglob(Text, type, S_FUNCTION, endlabel);
  Functionid = nameslot;

  // scan the '(' and ')' tokens
  lparen(file, curChar, token);
  rparen(file, curChar, token);

  // Get the AST tree for the compound statement
  tree = compound_statement(file, curChar, token);

  // If the function type isn't P_VOID, check that
  // the last AST operation in the compound statement
  // was a return statement
  if (type != P_VOID) {
    finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
    if (finalstmt == NULL || finalstmt->op != A_RETURN)
      fatal("No return for function with non-void type");
  }

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (mkastnode(A_FUNCTION, type, tree, NULL, NULL, nameslot));
}