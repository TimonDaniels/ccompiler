#include "scan.c"
#include "parse.c"

// maps 1 to 1 with the enum nodetypes
static int operatorPrecedense[] = {
    0, 10, 10,			// T_EOF, T_PLUS, T_MINUS
    20, 20,			    // T_STAR, T_SLASH
    30, 30,			    // T_EQ, T_NE
    40, 40, 40, 40		// T_LT, T_GT, T_LE, T_GE
  };

int getPrecedense(int op)
{
    int prec = operatorPrecedense[op];
    if (prec == 0)
    {
        printf("Error: unexpected token in getPrecedense\n");
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
    if (type == T_SEMI || type == T_RPAREN)
        return (left);

    current_precedense = getPrecedense(type);
    while (current_precedense > prec)
    {
        lexScan(file, curChar, token);
        right = binaryExpression(file, curChar, token, current_precedense);
        left = mkastnode(getNodeType(type), left, NULL, right, 0);
        
        // check for the end of the expression
        type = token->type;
        if (type == T_SEMI || type == T_RPAREN)
            return (left);
    
        // check the precedence of the next token
        current_precedense = operatorPrecedense[type];
    }

    return (left);
}