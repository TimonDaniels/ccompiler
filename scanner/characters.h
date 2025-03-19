#ifndef characters_h
#define characters_h

enum
{
    INT,
    PLUS,
    MINUS,
    TIMES,
    DEVIDE
};

struct CurChar
{
    char type;
    int processed;
};

#endif