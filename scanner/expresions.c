#include "scan.c"
#include "parse.c"

// maps 1 to 1 with the enum nodetypes
static int operatorPrecedense[] = {0, 0, 1, 1, 2, 2};

int getPrecedense(int op)
{
    int prec = operatorPrecedense[op];
    if (prec == 0)
    {
        printf("Error: unexpected token\n");
        exit(1);
    }
    return prec;
}

struct ASTnode *binaryExpression(FILE *file, struct CurChar *curChar, struct Token *token, int prec)
{
    struct ASTnode *left, *right;
    int type;
    int current_precedense;

    left = getNextIntNode(file, curChar, token);
    type = token->type;
    if (type == T_EOF)
    {
        return left;
    }

    current_precedense = getPrecedense(type);
    while (current_precedense > prec)
    {
        lexScan(file, curChar, token);
        right = binaryExpression(file, curChar, token, current_precedense);
        left = mkastnode(getNodeType(type), left, right, 0);
        type = token->type;

        current_precedense = operatorPrecedense[type];
    }

    return left;
}