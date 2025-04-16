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


struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *mid, struct ASTnode *right, int intvalue)
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
    new->mid = mid;
    new->right = right;
    new->v.intvalue = intvalue;

    return (new);
}

struct ASTnode *mkastleaf(int op, int intvalue)
{
    return (mkastnode(op, NULL, NULL, NULL, intvalue));
}

struct ASTnode *getNextIntNode(FILE *file, struct CurChar *curChar, struct Token *token)
{
    // assumes the next token is scanned and of type INT
    struct ASTnode *left;
    int id;

    switch (token->type)
    {
    case T_INTLIT:
        left = mkastleaf(A_INTLIT, token->value);
        break;
    case T_IDENT:
        // Check that this identifier exists
        id = findglob(Text);
        if (id == -1)
          printf("Unknown variable", Text);
    
        // Make a leaf AST node for it
        left = mkastleaf(A_IDENT, id);
        break;
    default:
        printf("Error: unexpected token, expected int \n");
        exit(1);
    }

    lexScan(file, curChar, token);
    return left;
}

struct ASTnode *createASTTree(FILE *file, struct CurChar *curChar, struct Token *token)
{
    struct ASTnode *new, *left, *right;
    int nodetype;

    lexScan(file, curChar, token);
    left = getNextIntNode(file, curChar, token);


    nodetype = getNodeType(token->type);

    right = createASTTree(file, curChar, token);

    new = mkastnode(nodetype, left, NULL, right, 0);
    return (new);
}

int intepretASTTree(struct ASTnode *n)
{
    int v1, v2;

    if (n->op == A_INTLIT)
    {
        return n->v.intvalue;
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
