#ifndef SCAN_C
#define SCAN_C

#include "global.h"
#include "defs.h"
#include "misc.c"

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

    while ((k = chrpos("0123456789", curChar->type)) >= 0)
    {
        value = 10 * value + k;
        nextc(file, curChar);
    }
    Putback = curChar->type;

    return value;
}

// Scan an identifier from the input file and
// store it in buf[]. Return the identifier's length
static int scanident(FILE *file, struct CurChar *curChar, char *buf, int lim) {
    int i = 0;
  
    // Allow digits, alpha and underscores
    while (isalpha(curChar->type) || isdigit(curChar->type) || '_' == (curChar->type)) {
      // Error if we hit the identifier length limit,
      // else append to buf[] and get next character
      if (lim - 1 == i) {
        printf("identifier too long on line %d\n", Line);
        exit(1);
      } else if (i < lim - 1) {
        buf[i++] = (curChar->type);
      }
      nextc(file, curChar);
    }
    // We hit a non-valid character, put it back.
    // NUL-terminate the buf[] and return the length
    Putback = curChar->type;
    buf[i] = '\0';
    return (i);
}

// Given a word from the input, return the matching
// keyword token number or 0 if it's not a keyword.
// Switch on the first letter so that we don't have
// to waste time strcmp()ing against all the keywords.
static int keyword(char *s) {
    switch (*s) {
      case 'p':
        if (!strcmp(s, "print"))
          return (T_PRINT);
        break;
      case 'i':
        if (!strcmp(s, "int"))
          return (T_INT);
        break;
    }
    return (0);
}

int lexScan(FILE *file, struct CurChar *curChar, struct Token *token)
{   
    int tokentype;
    skipChars(file, curChar);

    // check the character type
    switch (curChar->type)
    {
    case EOF:
        token->type = T_EOF;
        return 0;
    case '+':
        token->type = T_PLUS;
        break;
    case '-':
        token->type = T_MINUS;
        break;
    case '*':
        token->type = T_STAR;
        break;
    case '/':
        token->type = T_SLASH;
        break;
    case ';':
        token->type = T_SEMI;
        break;
    case '=':
      nextc(file, curChar);
      if ((curChar->type) == '=') {
        token->type = T_EQ;
      } else {
        Putback = curChar->type;
        token->type = T_ASSIGN;
      }
      break;
    case '!':
      nextc(file, curChar);
      if ((curChar->type) == '=') {
        token->type = T_NE;
      } else {
        fatalc("Unrecognised character", curChar->type);
      }
      break;
    case '<':
      nextc(file, curChar);
      if ((curChar->type) == '=') {
        token->type = T_LE;
      } else {
        Putback = curChar->type;
        token->type = T_LT;
      }
      break;
    case '>':
      nextc(file, curChar);
      if ((curChar->type) == '=') {
        token->type = T_GE;
      } else {
        Putback = curChar->type;
        token->type = T_GT;
      }
      break;
    default:
      // If it's a digit, scan the
      // literal integer value in
      if (isdigit(curChar->type)) {
        token->value = scanInt(file, curChar);
        token->type = T_INTLIT;
        break;
          } else if (isalpha(curChar->type) || '_' == curChar->type) {
        // Read in a keyword or identifier
        scanident(file, curChar, Text, TEXTLEN);
    
        // If it's a recognised keyword, return that token
        tokentype = keyword(Text);
        if (tokentype) {
          token->type = tokentype;
          break;
        }
        // Else it's an identifier, return T_IDENT
        token->type = T_IDENT;
        break;        
          }
          // The character isn't part of any recognised token, error
          printf("Unrecognised character %c on line %d\n", curChar->type, Line);
          exit(1);
      }
    return 1;
}

void match(FILE *file, struct CurChar *curChar, struct Token *token, int t, char *what) {
    if (token->type == t) {
      lexScan(file, curChar, token);
    } else {
      printf("%s expected on line %d\n", what, Line);
      exit(1);
    }
  }
  
  // Match a semicon and fetch the next token
void semi(FILE *file, struct CurChar *curChar, struct Token *token) {
    match(file, curChar, token, T_SEMI, ";");
}

void ident(FILE *file, struct CurChar *curChar, struct Token *token) {
    match(file, curChar, token, T_IDENT, "identifier");
}

#endif // SCAN_C