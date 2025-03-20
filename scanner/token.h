#ifndef TOKEN_H
#define TOKEN_H

enum
{
    T_EOF,
    INT,
    PLUS,
    MINUS,
    TIMES,
    DEVIDE
};

struct Token
{
    int type;
    int value;
};

#endif