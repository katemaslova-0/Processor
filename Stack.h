#ifndef STACK_H
#define STACK_H

typedef int StackEl;

struct Stack_t
{
    StackEl * data;
    StackEl size;
    int capacity;
};

enum StackErr_t
{
    NoError        =  1,
    StackPushLimit = (2 << 0),
    StackPopLimit  = (2 << 1),
    BadCapacity    = (2 << 2),
    NullStackPtr   = (2 << 3),
    NullDataPtr    = (2 << 4),
    ERROR          = (2 << 5)
};

#endif // STACK_H
