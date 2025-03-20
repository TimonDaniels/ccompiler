#include "characters.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Original work Copyright (c) 2019 Warren Toomey, GPL3
// Modified by Timon Daniels (2025)

static int chrpos(char *s, int c)
{
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

static void nextc(FILE *file, struct CurChar *curChar)
{
    // read the file
    curChar->type = fgetc(file);
    curChar->processed = 0;
}

static void skipSpaces(FILE *file, struct CurChar *curChar)
{
    if (curChar->processed)
    {
        nextc(file, curChar);
    }

    while (isspace(curChar->type))
    {
        curChar->processed = 1;
        nextc(file, curChar);
    }
}

static int scanInt(FILE *file, struct CurChar *curChar)
{
    // declare variables
    int k, value = 0;

    while ((k = chrpos("012345689", curChar->type)) >= 0)
    {
        value = 10 * value + k;
        curChar->processed = 1;
        nextc(file, curChar);
    }

    return value;
}

int lexScan(FILE *file, struct CurChar *curChar, struct Token *token)
{
    nextc(file, curChar);
    skipSpaces(file, curChar);

    // check the character type
    switch (curChar->type)
    {
    case EOF:
        token->type = T_EOF;
        curChar->processed = 1;
        return 0;
    case '+':
        token->type = PLUS;
        curChar->processed = 1;
        break;
    case '-':
        token->type = MINUS;
        curChar->processed = 1;
        break;
    case '*':
        token->type = TIMES;
        curChar->processed = 1;
        break;
    case '/':
        token->type = DEVIDE;
        curChar->processed = 1;
        break;
    default:
        if (isdigit(curChar->type))
        {
            token->type = INT;
            token->value = scanInt(file, curChar);
            curChar->processed = 1;
            break;
        }

        curChar->processed = 1;
    }

    switch (token->type)
    {
    case INT:
        printf("token: %d\n", token->value);
    default:
        printf("character: %c\n", curChar->type);
    }

    return 1;
}