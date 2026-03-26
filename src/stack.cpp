#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "Stack.h"
#include "StackFunc.h"

#define StackCtor(stk, capacity) if (StackCtor(stk, capacity) != NoError) {StackDtor(stk); return -1;}
#define StackPush(stk, value) if (StackPush(stk, value) != NoError) {return -1;} // вынести в хедер
#define StackPop(stk, value) if (StackPop(stk, value) != NoError) {return -1;}
#define CHECK_RET_VAL(func, ) if (func != NoError) {StackDtor(stk)return -1;}

int main (void)
{
    Stack_t stk1 = {};
    int x = 0, y = 0, z = 0, c = 0;

    StackCtor(&stk1, 5);

    StackPush(&stk1, 10);
    if (StackPush(&stk1, 20) != NoError) {
        StackDtor(&stk);
        return -1;
    }
    StackPush(&stk1, 30);
    StackPush(&stk1, 40);
    StackPush(&stk1, 50);

    StackPop(&stk1, &x);
    StackPop(&stk1, &y);
    StackPop(&stk1, &z);
    StackPop(&stk1, &c);
    StackPop(&stk1, &x);
    //StackPop(&stk1, &y);

    printf("Значение x: %d\n", x);
    printf("Значение y: %d\n", y);
    printf("Значение z: %d\n", z);
    printf("Значение c: %d\n", c);

    StackDtor(&stk1);
    return 0;
}
