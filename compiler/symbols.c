#ifndef SYMBOLS_C
#define SYMBOLS_C

#include "defs.h"
#include "global.h"

static int Globs = 0;

// Determine if the symbol s is in the global symbol table.
// returns the index of the symbol in the table, or -1 if not found.
int findglob(char *s) {
    int i;

    for (i = 0; i < Globs; i++) {
        if (*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
          return (i);
    }
    return (-1);
}

// returns the index of the next symbol in the symbol table
static int newglobindex(void) {
    int p;

    if ((p = Globs++) >= NSYMBOLS)
        printf("Too many symbols in symbol table\n");
    return (p);
}


// Add a symbol s to the global symbol table, and return its index.
int addglob(char *name, int type, int stype) {
    int p;

    if ((p = findglob(name)) != -1)
        return (p);
    
    p = newglobindex();
    Gsym[p].name = strdup(name);
    Gsym[p].type = type;
    Gsym[p].stype = stype;
    return (p);
}

#endif // SYMBOLS_C