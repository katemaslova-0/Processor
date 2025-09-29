#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "Stack.h"
#include "StackFunc.h"

#define STACK_OK(stk) if (StackErr(stk) != NoError) { \
        printf("\nStackDump() at %s:%d\n", __FILE__, __LINE__); \
        StackDump(stk, StackErr(stk)); return ERROR;
        }
#define MY_ASSERT(cond, cond2) if (!(cond)) {fprintf(stderr, "%s\nError in file %s in line %d\n", cond2, __FILE__, __LINE__); return ERROR;}

const StackEl POISON = 333;
const uint64_t CANARY_VALUE = 0xDEDOBEDAED;

StackErr_t StackCtor (Stack_t * stk, int capacity)
{
    MY_ASSERT(capacity > 0, "Некорректное значение размера массива");
    MY_ASSERT(stk != NULL, "Нулевой указатель на структуру");

    stk->data = (StackEl *) calloc ((size_t) capacity + 2, sizeof( StackEl));
    MY_ASSERT(stk->data != NULL, "Ошибка выделения памяти");

    //SetCanaryProtection(stk);

    stk->size = 0;
    stk->capacity = capacity;

    for (int count = 0; count < capacity; count++)
    {
        stk->data[count] = POISON;
    }

    STACK_OK(stk);

    return NoError;
}


/*StackErr_t SetCanaryProtection(Stack_t * stk)
{
    STACK_OK(stk);

    stk->data[0] = CANARY_VALUE;
    stk->data[stk->capacity + 1] = CANARY_VALUE;

    return NoError;
}*/


StackErr_t StackPush (Stack_t * stk, StackEl value)
{
    STACK_OK(stk);

    if (stk->size >= stk->capacity)
    {
        printf("Стек заполнен. Не удалось выполнить push.\n");
        return NoError;
    }

    stk->data[stk->size] = value;
    stk->size++;

    STACK_OK(stk);

    return NoError;
}


StackErr_t StackPop (Stack_t * stk, StackEl * value)
{
    assert(value);

    STACK_OK(stk);

    if (stk->size <= 0)
    {
        printf("Больше нет элементов в стеке. Не удалось выполнить pop.\n");
        return NoError;
    }

    *value = stk->data[stk->size - 1];
    stk->size--;

    STACK_OK(stk);

    return NoError;
}


int StackDtor (Stack_t * stk)
{
    MY_ASSERT(stk != NULL, "Нулевой указатель на структуру");

    free(stk->data);
    return 0;
}


void StackDump (Stack_t * stk, int sum_of_err)
{
    if (stk == NULL) {
        printf("Stack [NULL POINTER!]\n");
        return;
    }

    printf("Stack [%p]\n", stk); // отступы
    printf("{\n     size = %d\n     capacity = %d\n", stk->size, stk->capacity);
        if (stk->data != NULL)
        {
            printf("data[%p]\n", stk->data);
            printf("     {\n");
        }
        else
            printf("     data[NULL POINTER!]\n");

        if (stk->data != NULL)
        {
            for (int count = 0; count < stk->capacity; count++)
            {
                if (stk->data[count] != POISON)
                    printf("        *[%d] = %d\n", count, stk->data[count]);
                else
                    printf("        [%d] = %d (POISON)\n", count, stk->data[count]);
            }
            printf("     }\n");
        }


    // if (sum_of_err & StackPushLimit)
    //     printf("     Код ошибки: %d\n", StackPushLimit);
    // if (sum_of_err & StackPopLimit)
    //     printf("     Код ошибки: %d\n", StackPopLimit);
    // if (sum_of_err & BadCapacity)
    //     printf("     Код ошибки: %d\n", BadCapacity);
    // if (sum_of_err & NullStackPtr)
    //     printf("     Код ошибки: %d\n", NullStackPtr);
    // if (sum_of_err & NullDataPtr)
    //     printf("     Код ошибки: %d\n", NullDataPtr);

    printf("}\n");

    return 0;
}


int StackErr (Stack_t * stk)
{
    int sum_of_err = NoError;

    if (stk == NULL)
    {
        sum_of_err |= NullStackPtr;
    }
    else
    {
        if (stk->data == NULL)
        {
            sum_of_err |= NullDataPtr;
        }
        if (stk->capacity <= 0)
        {
            sum_of_err |= BadCapacity;
        }
        if (stk->size > stk->capacity)
        {
            sum_of_err |= StackPushLimit;
        }
        if (stk->size < 0) // поменять на 1
        {
            sum_of_err |= StackPopLimit;
        }
        if (stk->data[0] != ) // NULL?
    }
    return sum_of_err;
}
