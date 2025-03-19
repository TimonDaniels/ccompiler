#include "scan.h"
#include "characters.h"
#include "token.h"
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

    // open the file
    file = fopen("code-1.txt", "r");
    if (file == NULL)
    {
        printf("Error: file not found\n");
        return;
    }

    // read the file
    printf("Reading the file...\n");
    while (!feof(file))
    {
        lexScan(file, &curChar, &token);
        if (token.type == INT)
            printf("Token: %d, Value: %d\n", token.type, token.value);
        else
            printf("Token: %d, character: %c\n", token.type, curChar.type);
    }
    // close the file
    fclose(file);
}