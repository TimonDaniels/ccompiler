#include "global.h"
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
    if (Putback)
    {
        curChar->type = Putback;
        Putback = 0;
    }
    else
    {
        curChar->type = fgetc(file);
        if ('\n' == curChar->type)
        {
            Line++;
        }
    }
}

static void skipChars(FILE *file, struct CurChar *curChar)
{
    nextc(file, curChar);
    while (' ' == curChar->type || '\t' == curChar->type || '\n' == curChar->type || '\r' == curChar->type || '\f' == curChar->type)
    {
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
        nextc(file, curChar);
    }
    Putback = curChar->type;

    return value;
}

int lexScan(FILE *file, struct CurChar *curChar, struct Token *token)
{
    skipChars(file, curChar);

    // check the character type
    switch (curChar->type)
    {
    case EOF:
        token->type = T_EOF;
        return 0;
    case '+':
        token->type = PLUS;
        break;
    case '-':
        token->type = MINUS;
        break;
    case '*':
        token->type = TIMES;
        break;
    case '/':
        token->type = DEVIDE;
        break;
    default:
        if (isdigit(curChar->type))
        {
            token->type = INT;
            token->value = scanInt(file, curChar);
            break;
        }
    }

    switch (token->type)
    {
    case INT:
        printf("token: %d\n", token->value);
        break;
    default:
        printf("token: %c\n", curChar->type);
        break;
    }
    return 1;
}