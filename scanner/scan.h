#ifndef SCAN_H
#define SCAN_H

#include "characters.h"
#include "token.h"
#include <stdio.h>

int lexScan(FILE *file, struct CurChar *curChar, struct Token *token);

#endif