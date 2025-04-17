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
    id = addglob(Text, type, S_VARIABLE);
    cgglobsym(id);
    semi(file, curChar, token);
}

struct ASTnode* compound_statement(FILE *file, struct CurChar *curChar, struct Token *token);

// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(FILE *file, struct CurChar *curChar, struct Token *token) {
  struct ASTnode *tree;
  int nameslot;

  // Find the 'void', the identifier, and the '(' ')'.
  // For now, do nothing with them
  match(file, curChar, token, T_VOID, "void");
  ident(file, curChar, token);
  nameslot = addglob(Text, P_VOID, S_FUNCTION);
  lparen(file, curChar, token);
  rparen(file, curChar, token);

  // Get the AST tree for the compound statement
  tree = compound_statement(file, curChar, token);

  // Return an A_FUNCTION node which has the function's nameslot
  // and the compound statement sub-tree
  return (mkastnode(A_FUNCTION, P_VOID, tree, NULL, NULL, nameslot));
}