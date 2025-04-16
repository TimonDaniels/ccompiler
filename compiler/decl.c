#include "global.h"
#include "defs.h"
#include "scan.c"
#include "symbols.c"


// Parse the declaration of a variable
void var_declaration(FILE *file, struct CurChar *curChar, struct Token *token) {

    // Ensure we have an 'int' token followed by an identifier
    // and a semicolon. Text now has the identifier's name.
    // Add it as a known identifier
    match(file, curChar, token, T_INT, "int");
    ident(file, curChar, token);
    addglob(Text);
    cgglobsym(Text);
    semi(file, curChar, token);
  }