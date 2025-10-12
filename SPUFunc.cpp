#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>

#include "Data.h"
#include "StackFunc.h"
#include "SPUFunc.h"
#include "Processor.h"

ProcErr_t ReadFileToCode(const char * filename, int ** code, int * size_of_code)
{
    assert(filename);
    assert(code);
    assert(size_of_code);

    FILE * fp = fopen(filename, "r");

    struct stat st;
    stat(filename, &st);
    *size_of_code = (int)st.st_size;
    *code = (int *) calloc((size_t)*size_of_code, sizeof(int));

    if (*code == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return PROC_ERROR;
    }

    int check_return = 0;
    int count = 0;
    for (count = 0; check_return != EOF; count++)
        check_return = fscanf(fp, "%d", &(*code)[count]);

    *size_of_code = count + 1;
    fclose(fp);

    return NoProcError;
}

ProcErr_t InitProcessor(SPU_t * proc, int * code, int size_of_code)
{
    assert(proc);
    assert(code);

    Stack_t stk = {}; // инициализация стека
    VerifyStackCtor(&stk, SIZE_OF_STACK);

    proc->stk = stk; // запись элементов в структуру процессора
    proc->code = code;
    proc->cmd_count = 0;
    proc->reg[NUM_OF_REGS] = {};
    
    PROC_OK(proc, size_of_code);
    return NoProcError;
}


int ProcErr(SPU_t * proc, int size_of_code) // 
{
    int sum_of_err = NoProcError;

    if (proc == NULL)
    {
        sum_of_err |= NullProcPtr;
        return sum_of_err;
    }
    if (proc->code == NULL)
    {
        sum_of_err |= NullCodePtr;
        return sum_of_err;
    }
    if (proc->cmd_count >= size_of_code)
    {
        sum_of_err |= CodeArrayOverflow;
    }
    if (proc->cmd_count < 0)
    {
        sum_of_err |= InvalidCount;
    }

    return sum_of_err;
}


void OutputProcErrorCode(int sum_of_err)
{
    if (sum_of_err & NullProcPtr)
        printf("     Код ошибки: %d (NullProcPtr)\n", NullProcPtr);
    if (sum_of_err & CodeArrayOverflow)
        printf("     Код ошибки: %d (CodeArrayOverflow)\n", CodeArrayOverflow);
    if (sum_of_err & InvalidCount)
        printf("     Код ошибки: %d (InvalidCount)\n", InvalidCount);
    if (sum_of_err & NullCodePtr)
        printf("     Код ошибки: %d (NullCodePtr)\n", NullCodePtr);
}


int ProcDump(SPU_t * proc, int size_of_code)
{
    if (proc == NULL)
    {
        printf("Proc [NULL POINTER!]\n");
        return 0;
    }
    printf("Proc [%p]\n", proc);
    
    StackDump(&(proc->stk));

    if (proc->code == NULL)
    {
        printf("Code [NULL POINTER!]\n");
        return 0;
    }

    printf("\nCode [%p]: ", &(proc->code));
    for (int count = 0; count < size_of_code; count++)
    {
        printf("[%d]  ", *(proc->code + count));
        if (count == proc->cmd_count)
        {
            printf("  << here  ");
        }
    }

    printf("\nCommand counter: %d\n", proc->cmd_count);
    printf("Regs [%p]: ", proc->reg);
    for (int count = 0; count < NUM_OF_REGS; count++)
    {
        printf("[%d]  ", (proc->reg)[count]);
    }
    
    return 0;
}


ProcErr_t Calc(SPU_t * proc, int size_of_code)
{
    PROC_OK(proc, size_of_code);

    bool if_end_of_calc = false;

    while (if_end_of_calc != true)
    {
        if ((proc->code)[proc->cmd_count] == PUSH)
        {
            proc->cmd_count++;
            VerifyStackPush(&(proc->stk), (proc->code)[proc->cmd_count]);
        }
        else if ((proc->code)[proc->cmd_count] == ADD)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            VerifyStackPush(&(proc->stk), x + y);
        }
        else if ((proc->code)[proc->cmd_count] == SUB)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            VerifyStackPush(&(proc->stk), y - x);
        }
        else if ((proc->code)[proc->cmd_count] == MUL)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            VerifyStackPush(&(proc->stk), x * y);
        }
        else if ((proc->code)[proc->cmd_count] == JB)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            if (x > y)
                proc->cmd_count = (proc->code)[proc->cmd_count + 1];
            else
                proc->cmd_count++;
        }
        else if ((proc->code)[proc->cmd_count] == JBE)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            if (x >= y)
                proc->cmd_count = (proc->code)[proc->cmd_count + 1];
            else
                proc->cmd_count++;
        }
        else if ((proc->code)[proc->cmd_count] == JA)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            if (x < y)
                proc->cmd_count = (proc->code)[proc->cmd_count + 1];
            else
                proc->cmd_count++;
        }
        else if ((proc->code)[proc->cmd_count] == JAE)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            if (x <= y)
                proc->cmd_count = (proc->code)[proc->cmd_count + 1];
            else
                proc->cmd_count++;
        }
        else if ((proc->code)[proc->cmd_count] == JE)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            if (x == y)
                proc->cmd_count = (proc->code)[proc->cmd_count + 1];
            else
                proc->cmd_count++;
        }
        else if ((proc->code)[proc->cmd_count] == JNE)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &x);
            VerifyStackPop(&(proc->stk), &y);
            if (x != y)
                proc->cmd_count = (proc->code)[proc->cmd_count + 1];
            else
                proc->cmd_count++;
        }
        else if ((proc->code)[proc->cmd_count] == IN)
        {
            int x = 0;
            printf("Введите число: ");
            scanf("%d", &x);
            VerifyStackPush(&(proc->stk), x);
        }
        else if ((proc->code)[proc->cmd_count] == DIV)
        {
            int x = 0, y = 0;
            VerifyStackPop(&(proc->stk), &y);
            VerifyStackPop(&(proc->stk), &x);
            if (y == 0)
            {
                printf("ERROR: Division by zero\n");
                return PROC_ERROR;
            }
            VerifyStackPush(&(proc->stk), x / y);
        }
        else if ((proc->code)[proc->cmd_count] == SQRT)
        {
            int x = 0;
            VerifyStackPop(&(proc->stk), &x);
            if (x < 0)
            {
                printf("ERROR: Cannot SQRT negative value\n");
                return PROC_ERROR;
            }
            x = (int)sqrt((double)x);
            VerifyStackPush(&(proc->stk), x);
        }
        else if ((proc->code)[proc->cmd_count] == POW)
        {
            int x = 0;
            VerifyStackPop(&(proc->stk), &x);
            proc->cmd_count++;
            int pow = (proc->code)[proc->cmd_count];
            int start_x = x;
            for (int count = 1; count < pow; count++)
                x *= start_x;
            VerifyStackPush(&(proc->stk), x);
        }
        else if ((proc->code)[proc->cmd_count] == OUT)
        {
            int x = 0;
            VerifyStackPop(&(proc->stk), &x);
            printf("Answer: %d\n", x);
        }
        else if ((proc->code)[proc->cmd_count] == HLT)
        {
            if_end_of_calc = true;
        }
        else if ((proc->code)[proc->cmd_count] == PUSHREG)
        {
            proc->cmd_count++;
            int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII;
            VerifyStackPush(&(proc->stk), (proc->reg)[num_of_reg]);
        }
        else if ((proc->code)[proc->cmd_count] == POPREG)
        {
            int x = 0;
            VerifyStackPop(&(proc->stk), &x);
            proc->cmd_count++;
            int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII;
            (proc->reg)[num_of_reg] = x; // check  
        }
        else
        {
            printf("Invalid command\n");
            ProcDtor(proc);
            return PROC_ERROR;
        }

        proc->cmd_count++;
        PROC_OK(proc, size_of_code);
        /*ProcDump(proc, size_of_code);
        while (getchar() != '\n')
            continue;*/
    }

    return NoProcError;
}


void ProcDtor (SPU_t * proc)
{
    assert(proc);
    assert(proc->code);

    free(proc->code);
    StackDtor(&(proc->stk));
}