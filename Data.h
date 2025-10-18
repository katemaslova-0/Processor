#ifndef DATA_H
#define DATA_H

enum CmdNum
{
    PUSH        = 0,
    ADD         = 1,
    SUB         = 2,
    MUL         = 3,
    DIV         = 4,
    POW         = 5,
    REMAINDER   = 6,
    OUT         = 7,
    HLT         = 8,
    PUSHREG     = 9,
    POPREG      = 10,
    JB          = 11,
    JBE         = 12,
    JA          = 13,
    JAE         = 14,
    JE          = 15,
    JNE         = 16,
    JMP         = 17,
    SQRT        = 18,
    IN          = 19,
    CALL        = 20,
    RET         = 21,
    PUSHM       = 22,
    POPM        = 23,
    DRAW        = 24
};

#endif // DATA_H
