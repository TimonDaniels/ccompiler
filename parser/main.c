#include "characters.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static int chrpos(char *s, int c)
{
    char *p;

    p = strchr(s, c);
    return (p ? p - s : -1);
}

void nextc(FILE *file, struct CurChar *curChar)
{
    // read the file
    curChar->type = fgetc(file);
    curChar->processed = 0;
}

int parseInt(FILE *file, struct CurChar *curChar)
{
    // declare variables
    int k, value = 0;

    if ((k = chrpos("012345689", curChar->type)) >= 0)
    {
        value = 10 * value + k;
        nextc(file, curChar);
        curChar->processed = 1;
    }

    return value;
}

int parseToken(FILE *file, struct CurChar *curChar, struct Token *token)
{
    nextc(file, curChar);

    // check the character type
    switch (curChar->type)
    {
    case EOF:
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
            token->value = parseInt(file, curChar);
            break;
        }

        curChar->processed = 1;
    }

    return 1;
}

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
        parseToken(file, &curChar, &token);
        if (token.type == INT)
            printf("Token: %d, Value: %d\n", token.type, token.value);
        else
            printf("Token: %d, character: %d\n", token.type, curChar.type);
    }
    // close the file
    fclose(file);
}