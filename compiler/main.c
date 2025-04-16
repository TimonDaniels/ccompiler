#define extern_
#include "global.h"
#include "defs.h"
#include "gen_asm.c"
#include "stmt.c"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Original work Copyright (c) 2019 Warren Toomey, GPL3
// Modified by Timon Daniels (2025)

void main()
{
    // declare variables
    FILE *InFile;
    struct CurChar curChar;
    struct Token token;
    struct ASTnode *rootnode;

    // open the file
    InFile = fopen("code-1.txt", "r");
    if (InFile == NULL)
    {
        fprintf(stderr, "Error: file not found: %s\n", strerror(errno));
        exit(1);
    }

    Outfile = fopen("assembly.s", "w");
    if (Outfile == NULL)
    {
        fprintf(stderr, "Error: could not open assembly file: %s\n", strerror(errno));
        fclose(InFile);
        exit(1);
    }

    // read the file
    printf("Reading the file...\n");
    lexScan(InFile, &curChar, &token);
    cgpreamble();
    printf("Generating assembly code...\n");
    statements(InFile, &curChar, &token);
    cgpostamble();

    // close the Outfile
    fclose(Outfile);
    exit(0);
}