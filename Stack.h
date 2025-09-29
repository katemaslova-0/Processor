#ifndef STACK_H
#define STACK_H

typedef char* StackEl;

struct Stack_t
{
    StackEl * data;
    StackEl size; // int
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
    ERROR          = (1 << 6)
};

#endif // STACK_H
