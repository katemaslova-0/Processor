#ifndef STACKFUNC_H
#define STACKFUNC_H

StackErr_t StackCtor (Stack_t * stk, int capacity);
StackErr_t StackPush (Stack_t * stk, StackEl value);
StackErr_t StackPop (Stack_t * stk, StackEl * value);
int StackDtor (Stack_t * stk);
int StackErr (Stack_t * stk);
int StackDump (Stack_t * stk, int sum_of_err);
//StackErr_t SetCanaryProtection(Stack_t * stk);

#endif // STACKFUNC_H
