#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int getNodeType(int op)
{
    switch (op)
    {
        case T_PLUS:
            return (A_ADD);
        case T_MINUS:
            return (A_SUBTRACT);
        case T_STAR:
            return (A_MULTIPLY);
        case T_SLASH:
            return (A_DIVIDE);
        default:
            printf("Error: unexpected token in getNodeType\n");
            exit(1);
    }
}


struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *right, int intvalue)
{
    struct ASTnode *new;

    new = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if (new == NULL)
    {
        printf("Ran out of memory\n");
        exit(1);
    }

    new->op = op;
    new->left = left;
    new->right = right;
    new->intvalue = intvalue;

    return (new);
}

struct ASTnode *mkastleaf(int op, int intvalue)
{
    return (mkastnode(op, NULL, NULL, intvalue));
}

struct ASTnode *getNextIntNode(FILE *file, struct CurChar *curChar, struct Token *token)
{
    // assumes the next token is scanned and of type INT
    struct ASTnode *left;
    switch (token->type)
    {
    case T_INTLIT:
        left = mkastleaf(A_INTLIT, token->value);
        lexScan(file, curChar, token);
        return left;
    default:
        printf("Error: unexpected token, expected int \n");
        exit(1);
    }
}

struct ASTnode *createASTTree(FILE *file, struct CurChar *curChar, struct Token *token)
{
    struct ASTnode *new, *left, *right;
    int nodetype;

    lexScan(file, curChar, token);
    left = getNextIntNode(file, curChar, token);

    nodetype = getNodeType(token->type);

    right = createASTTree(file, curChar, token);

    new = mkastnode(nodetype, left, right, 0);
    return (new);
}

int intepretASTTree(struct ASTnode *n)
{
    int v1, v2;

    if (n->op == A_INTLIT)
    {
        return n->intvalue;
    }

    v1 = intepretASTTree(n->left);
    v2 = intepretASTTree(n->right);

    switch (n->op)
    {
    case A_ADD:
        return v1 + v2;
    case A_SUBTRACT:
        return v1 - v2;
    case A_MULTIPLY:
        return v1 * v2;
    case A_DIVIDE:
        return v1 / v2;
    default:
        printf("Error: unexpected token in interpretASTTree\n");
        exit(1);
    }
}