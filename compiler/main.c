#define extern_
#include "global.h"
#include "defs.h"
#include "gen_asm.c"
#include "statements.c"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Original work Copyright (c) 2019 Warren Toomey, GPL3
// Modified by Timon Daniels (2025)
// Initialise global variables
static void init() {
    Line = 1;
    Putback = '\n';
}

static void usage(char *prog) {
    fprintf(stderr, "Usage: %s infile\n", prog);
    exit(1);
}

void main(int argc, char *argv[])
{
    // declare variables
    FILE *Infile;
    struct CurChar curChar;
    struct Token token;
    struct ASTnode *rootnode;
    
    if (argc != 2)
        usage(argv[0]);

    init();

    // open the file
    Infile = fopen(argv[1], "r");
    if (Infile == NULL)
    {
        fprintf(stderr, "Error: file %s not found: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    Outfile = fopen("assembly.s", "w");
    if (Outfile == NULL)
    {
        fprintf(stderr, "Error: could not open assembly file: %s\n", strerror(errno));
        fclose(Infile);
        exit(1);
    }

    // read the file
    printf("Reading the file...\n");
    lexScan(Infile, &curChar, &token);
    
    printf("Generating assembly code...\n");
    cgpreamble();                                                   // Output the preamble
    while (1) {                                                     // Parse a function and
      rootnode = function_declaration(Infile, &curChar, &token);    // generate the assembly code for it
      genAST(rootnode, NOREG, 0);                                   // generate the assembly code for it
      if (token.type == T_EOF)                                      // Stop when we have reached EOF
        break;
    }

    // close the Outfile
    fclose(Outfile);
    exit(0);
}