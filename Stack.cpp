#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Data.h"
#include "StackFunc.h"

#define STACK_OK(stk) if (StackErr(stk) != NoError) { \
        printf("\nStackDump() at %s:%d\n", __FILE__, __LINE__); \
        int result = StackErr(stk); \
        StackDump(stk); \
        OutputErrorCode(result); \
        return ERROR; \
        }

StackErr_t StackCtor (Stack_t * stk, int capacity)
{
    MY_ASSERT(capacity > 0, "Некорректное значение размера массива");
    MY_ASSERT(stk != NULL, "Нулевой указатель на структуру");

    stk->data = (StackEl *) calloc ((size_t) (capacity + 2), sizeof(StackEl));
    MY_ASSERT(stk->data != NULL, "Ошибка выделения памяти");

    stk->size = 1;
    stk->capacity = capacity;

    SetCanaryProtection(stk);

    for (int count = 1; count < capacity + 1; count++)
    {
        stk->data[count] = POISON;
    }

    STACK_OK(stk);

    return NoError;
}


void SetCanaryProtection (Stack_t * stk)
{
    assert(stk);

    *(stk->data) = CANARY_VALUE;
    *(stk->data + stk->capacity + 1) = CANARY_VALUE;
}


StackErr_t StackPush (Stack_t * stk, StackEl value)
{
    STACK_OK(stk);

    if (stk->size > stk->capacity)
    {
        if (StackRealloc(stk) != NoError)
            return ERROR;
    }

    stk->data[stk->size] = value;
    stk->size++;

    STACK_OK(stk);

    return NoError;
}


StackErr_t StackRealloc(Stack_t * stk)
{
    assert(stk);

    int old_capacity = stk->capacity;
    stk->capacity *= 2;
    stk->data = (StackEl *) realloc (stk->data, (size_t)(stk->capacity + 2) * sizeof(StackEl));

    for (int count = old_capacity + 1; count < stk->capacity + 1; count++)
        *(stk->data + count) = POISON;

    SetCanaryProtection(stk);

    if (stk->data == NULL)
        return ERROR;

    return NoError;
}


StackErr_t StackPop (Stack_t * stk, StackEl * value)
{
    assert(value);

    STACK_OK(stk);

    stk->size--;

    if (stk->size <= 0)
    {
        printf("Больше нет элементов в стеке. Не удалось выполнить pop.\n");
        return NoError;
    }

    *value = stk->data[stk->size];
    stk->data[stk->size] = POISON; // добавлено 7.10

    STACK_OK(stk);

    return NoError;
}


int StackDtor (Stack_t * stk)
{
    MY_ASSERT(stk != NULL, "Нулевой указатель на структуру");

    free(stk->data);
    return 0;
}


int StackDump (Stack_t * stk)
{
    if (stk == NULL)
    {
        printf("Stack [NULL POINTER!]\n");
        return 0;
    }

    printf("Stack [%p]\n", stk);
    printf("{\n     size = %d\n     capacity = %d\n", stk->size, stk->capacity);
    
    if (stk->data != NULL)
    {
        printf("     data[%p]\n", stk->data);
        printf("     {\n");
    }
    else
    {
        printf("     data[NULL POINTER!]\n");
        printf("}\n");
    }
    if (stk->data != NULL)
    {
        for (int count = 1; count < stk->capacity + 1; count++)
        {
            if (stk->data[count] != POISON)
                printf("        *[%d] = %d\n", count, stk->data[count]);
            else
                printf("        [%d] = %d (POISON)\n", count, stk->data[count]);
        }
        printf("     }\n");
    }
    printf("}\n");

    return 0;
}


void OutputErrorCode(int sum_of_err)
{
    if (sum_of_err & StackPushLimit)
        printf("     Код ошибки: %d (StackPushLimit)\n", StackPushLimit);
    if (sum_of_err & StackPopLimit)
        printf("     Код ошибки: %d (StackPopLimit)\n", StackPopLimit);
    if (sum_of_err & BadCapacity)
        printf("     Код ошибки: %d (BadCapacity)\n", BadCapacity);
    if (sum_of_err & NullStackPtr)
        printf("     Код ошибки: %d (NullStackPtr)\n", NullStackPtr);
    if (sum_of_err & NullDataPtr)
        printf("     Код ошибки: %d (NullDataPtr)\n", NullDataPtr);
    if (sum_of_err & StackOverflow)
        printf("     Код ошибки: %d (StackOverflow)\n", StackOverflow);
}


int StackErr (Stack_t * stk)
{
    int sum_of_err = NoError;

    if (stk == NULL)
    {
        sum_of_err |= NullStackPtr;
        return sum_of_err;
    }
    if (stk->capacity <= 0)
    {
        sum_of_err |= BadCapacity;
    }
    if (stk->size > stk->capacity + 1)
    {
        sum_of_err |= StackPushLimit;
    }
    if (stk->size < 1)
    {
        sum_of_err |= StackPopLimit;
    }
    if (stk->data == NULL)
    {
        sum_of_err |= NullDataPtr;
        return sum_of_err;
    }
    if ((*(stk->data) != CANARY_VALUE) || (*(stk->data + stk->capacity + 1)) != CANARY_VALUE)
    {
        sum_of_err |= StackOverflow;
    }

    return sum_of_err;
}
