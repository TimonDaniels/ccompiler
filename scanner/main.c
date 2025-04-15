#define extern_
#include "global.h"
#include "defs.h"
#include "expresions.c"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Original work Copyright (c) 2019 Warren Toomey, GPL3
// Modified by Timon Daniels (2025)

void main()
{
    // declare variables
    FILE *file;
    struct CurChar curChar;
    struct Token token;
    struct ASTnode *rootnode;

    // open the file
    file = fopen("code-1.txt", "r");
    if (file == NULL)
    {
        printf("Error: file not found\n");
        return;
    }

    // read the file
    printf("Reading the file...\n");
    lexScan(file, &curChar, &token);
    rootnode = binaryExpression(file, &curChar, &token, 0);
    printf("Value calculated: %d\n", intepretASTTree(rootnode));

    // close the file
    fclose(file);
}