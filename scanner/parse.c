#include "characters.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

enum nodetype
{
    ADD,
    SUB,
    MUL,
    DIV,
    VAL,
};

int getNodeType(int op)
{
    switch (op)
    {
    case PLUS:
        return ADD;
    case MINUS:
        return SUB;
    case TIMES:
        return MUL;
    case DEVIDE:
        return DIV;
    default:
        printf("Error: unexpected token\n");
        exit(1);
    }
}

struct ASTnode
{
    int op;
    struct ASTnode *left;
    struct ASTnode *right;
    int intvalue;
};

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

struct ASTnode *createASTTree(FILE *file, struct CurChar *curChar, struct Token *token)
{
    struct ASTnode *new, *left, *right;
    int nodetype;

    lexScan(file, curChar, token);
    if (token->type == INT)
    {
        left = mkastleaf(VAL, token->value);
        lexScan(file, curChar, token);
        if (token->type == T_EOF)
        {
            return (left);
        }
    }
    else
    {
        printf("Error: unexpected token, expected int \n");
    }

    nodetype = getNodeType(token->type);

    right = createASTTree(file, curChar, token);

    new = mkastnode(nodetype, left, right, 0);
    return (new);
}

int intepretASTTree(struct ASTnode *n)
{
    int v1, v2;

    if (n->op == VAL)
    {
        return n->intvalue;
    }

    v1 = intepretASTTree(n->left);
    v2 = intepretASTTree(n->right);

    switch (n->op)
    {
    case ADD:
        return v1 + v2;
    case SUB:
        return v1 - v2;
    case MUL:
        return v1 * v2;
    case DIV:
        return v1 / v2;
    default:
        printf("Error: unexpected token\n");
        exit(1);
    }
}