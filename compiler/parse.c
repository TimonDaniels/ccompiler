#include "defs.h"
#include "symbols.c"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int getNodeType(int op)
{
    if (op > T_EOF && op < T_INTLIT)
        return (op);
    printf("Syntax error: unexpected token\n");
    exit(1);
}


struct ASTnode *mkastnode(int op, int type, struct ASTnode *left, struct ASTnode *mid, struct ASTnode *right, int intvalue)
{
    struct ASTnode *new;

    new = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if (new == NULL)
    {
        printf("Ran out of memory\n");
        exit(1);
    }

    new->op = op;
    new->type = type;
    new->left = left;
    new->mid = mid;
    new->right = right;
    new->v.intvalue = intvalue;

    return (new);
}

struct ASTnode *mkastleaf(int op, int type, int intvalue)
{
    return (mkastnode(op, type, NULL, NULL, NULL, intvalue));
}

struct ASTnode *getNextIntNode(FILE *file, struct CurChar *curChar, struct Token *token)
{
    // assumes the next token is scanned and of type INT
    struct ASTnode *left;
    int id;

    switch (token->type)
    {
    case T_INTLIT:
        if ((token->value) >= 0 && (token->value) <= 255)
            left = mkastleaf(A_INTLIT, P_CHAR, token->value);
        else
            printf("Integer literal out of range\n");
            left = mkastleaf(A_INTLIT, P_INT, token->value);
        break;
    case T_IDENT:
        // Check that this identifier exists
        id = findglob(Text);
        if (id == -1)
          printf("Unknown variable", Text);
    
        // Make a leaf AST node for it
        left = mkastleaf(A_IDENT, Gsym[id].type, id);
        break;
    default:
        fatald("Syntax error, token", token->type);
    }

    lexScan(file, curChar, token);
    return left;
}
