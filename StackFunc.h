#ifndef STACKFUNC_H
#define STACKFUNC_H

typedef int StackEl;

const StackEl POISON = 333;
const StackEl CANARY_VALUE = 409349;
const int SIZE_OF_COMMAND = 10;
const int SIZE_OF_STACK = 10;

struct Stack_t
{
    StackEl * data;
    int size;
    int capacity;
};

enum StackErr_t
{
    NoError        = (1 << 0),
    StackPushLimit = (1 << 1),
    StackPopLimit  = (1 << 2),
    BadCapacity    = (1 << 3),
    NullStackPtr   = (1 << 4),
    NullDataPtr    = (1 << 5),
    StackOverflow  = (1 << 6),
    ERROR          = (1 << 7)
};

#define VerifyStackCtor(stk, capacity) if (StackCtor(stk, capacity) != NoError) {StackDtor(stk); return ProcError;}
#define VerifyStackPush(stk, value) if (StackPush(stk, value) != NoError) {StackDtor(stk); return ProcError;}
#define VerifyStackPop(stk, value) if (StackPop(stk, value) != NoError) {StackDtor(stk); return ProcError;}

StackErr_t StackCtor(Stack_t * stk, int capacity);
StackErr_t StackPush(Stack_t * stk, StackEl value);
StackErr_t StackPop(Stack_t * stk, StackEl * value);
int StackDtor (Stack_t * stk);
int StackErr (Stack_t * stk);
int StackDump (Stack_t * stk);
void SetCanaryProtection(Stack_t * stk);
void Calc(FILE * fp, Stack_t * stk);
void OutputErrorCode(int sum_of_err);
StackErr_t StackRealloc(Stack_t * stk);

#endif // STACKFUNC_H
