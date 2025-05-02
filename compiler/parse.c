#include "defs.h"
#include "symbols.c"
#include "types.c"

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

struct ASTnode *binaryExpression(FILE *file, struct CurChar *curChar, struct Token *token, int prec);

struct ASTnode* funccall(FILE *file, struct CurChar *curChar, struct Token *token) {
    struct ASTnode *tree;
    int id;
  
    id = findglob(Text);
    if (id == -1) {
      fatals("Undeclared function %s", Text);
    }
  
    lparen(file, curChar, token);
  
    tree = binaryExpression(file, curChar, token, 0);
  
    tree = mkastnode(A_FUNCCALL, Gsym[id].type, tree, NULL, NULL, id);
    rparen(file, curChar, token);
  
    return (tree);
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
        else {
            printf("Integer literal out of range\n");
            left = mkastleaf(A_INTLIT, P_INT, token->value);
        }
        break;
    case T_IDENT:
        // This could be a variable or a function call.
        // Scan in the next token to find out
        lexScan(file, curChar, token);

        // It's a '(', so a function call
        if (token->type == T_LPAREN)
            return (funccall(file, curChar, token));

        // Not a function call, so reject the new token
        reject_token(token);

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

// Parse a prefix expression and return 
// a sub-tree representing it.
struct ASTnode *prefix(FILE *file, struct CurChar *curChar, struct Token *token) {
    struct ASTnode *tree;

    switch (token->type) {
      case T_AMPER:
        // Get the next token and parse it
        // recursively as a prefix expression
        lexScan(file, curChar, token);
        tree = getNextIntNode(file, curChar, token);
  
        // Ensure that it's an identifier
        if (tree->op != A_IDENT)
          fatal("& operator must be followed by an identifier");
  
        // Now change the operator to A_ADDR and the type to
        // a pointer to the original type
        tree->op = A_ADDR; 
        tree->type = pointer_to(tree->type);
        break;

      case T_STAR:
        // Get the next token and parse it
        // recursively as a prefix expression
        lexScan(file, curChar, token);
        tree = getNextIntNode(file, curChar, token);
  
        // For now, ensure it's either another deref or an
        // identifier
        if (tree->op != A_IDENT && tree->op != A_DEREF)
          fatal("* operator must be followed by an identifier or *");
  
        // Prepend an A_DEREF operation to the tree
        tree = mkastnode(A_DEREF, value_at(tree->type), tree, NULL, NULL, 0);
        break;

      default:
        tree = getNextIntNode(file, curChar, token);
    }
    return (tree);
  }
