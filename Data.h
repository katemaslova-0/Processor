#ifndef DATA_H
#define DATA_H

#include "SPUFunc.h"
#include "Processor.h"

#define MY_ASSERT(cond, cond2) if (!(cond)) {fprintf(stderr, "%s\nError in file %s in line %d\n", cond2, __FILE__, __LINE__); return ERROR;}

enum CmdNum
{
    PUSH    = 1,
    ADD     = 2,
    SUB     = 3,
    MUL     = 4,
    DIV     = 5,
    POW     = 6,
    OUT     = 7,
    HLT     = 8,
    PUSHREG = 9,
    POPREG  = 10,
    JB      = 11,
    JBE     = 12,
    JA      = 13,
    JAE     = 14,
    JE      = 15,
    JNE     = 16,
    SQRT    = 17,
    IN      = 18
};

#endif // DATA_H
