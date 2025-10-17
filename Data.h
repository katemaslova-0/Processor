#ifndef DATA_H
#define DATA_H

enum CmdNum
{
    PUSH    = 0,
    ADD     = 1,
    SUB     = 2,
    MUL     = 3,
    DIV     = 4,
    POW     = 5,
    OUT     = 6,
    HLT     = 7,
    PUSHREG = 8,
    POPREG  = 9,
    JB      = 10,
    JBE     = 11,
    JA      = 12,
    JAE     = 13,
    JE      = 14,
    JNE     = 15,
    SQRT    = 16,
    IN      = 17,
    CALL    = 18,
    RET     = 19,
    PUSHM   = 20,
    POPM    = 21,
    DRAW    = 22
};

#endif // DATA_H
