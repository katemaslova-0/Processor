#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Data.h"
#include "StackFunc.h"
#include "SPUFunc.h"

const int NUM_OF_REGS = 4;

struct SPU_t
{
    Stack_t stk;
    int * code;
    int cmd_count;
    int reg[NUM_OF_REGS];
};

enum RegName
{
    AX = 65,
    BX = 66,
    CX = 67,
    DX = 68
};

enum ProcErr_t
{
    NoProcError       = (1 << 0),
    PROC_ERROR        = (1 << 1),
    NullProcPtr       = (1 << 2),
    CodeArrayOverflow = (1 << 3),
    InvalidCount      = (1 << 4),
    NullCodePtr       = (1 << 5)
};

#endif // PROCESSOR_H