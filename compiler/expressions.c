#ifndef EXPRESSIONS_C
#define EXPRESSIONS_C

#include "scan.c"
#include "parse.c"
#include "types.c"



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
    int tokentype;
    int lefttype, righttype;
    int current_precedense;

    left = getNextIntNode(file, curChar, token);
    tokentype = token->type;
    if (tokentype == T_SEMI || tokentype == T_RPAREN)
        return (left);

    current_precedense = getPrecedense(tokentype);
    while (current_precedense > prec)
    {
        lexScan(file, curChar, token);
        right = binaryExpression(file, curChar, token, current_precedense);

        // Ensure the two types are compatible.
        lefttype = left->type;
        righttype = right->type;
        if (!type_compatible(&lefttype, &righttype, 0))
        fatal("Incompatible types");

        // Widen either side if required. type vars are A_WIDEN now
        if (lefttype)
            left = mkastnode(lefttype, right->type, left, NULL, NULL, 0);
        if (righttype)
            right = mkastnode(righttype, left->type, right, NULL, NULL, 0);
        
        // join subtrees together
        left = mkastnode(getNodeType(tokentype), left->type, left, NULL, right, 0);
        
        // check for the end of the expression
        tokentype = token->type;
        if (tokentype == T_SEMI || tokentype == T_RPAREN)
            return (left);
    
        // check the precedence of the next token
        current_precedense = operatorPrecedense[tokentype];
    }

    return (left);
}

#endif // EXPRESSIONS_C