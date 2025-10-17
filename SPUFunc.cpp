#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>

#include "Data.h"
#include "StackFunc.h"
#include "SPUFunc.h"
#include "Processor.h"

const int A_ASCII = 65;

#define PROC_OK(proc)   if (ProcErr(proc) != NoProcError) { \
                        printf("ProcDump at %s:%d\n", __FILE__, __LINE__); \
                        int result = ProcErr(proc); \
                        ProcDump(proc); \
                        OutputProcErrorCode(result); \
                        ProcDtor(proc); \
                        return ProcError; \
                        }

int GetFileSize (const char * filename)
{
    assert(filename);

    struct stat st;
    stat(filename, &st);
    return (int)st.st_size;
}

ProcErr_t ReadFileToCode(const char * filename, int ** code, int * size_of_code)
{
    assert(filename);
    assert(code);
    assert(size_of_code);

    FILE * fp = fopen(filename, "r");
    
    if (fp == NULL)
    {
        printf("Ошибка при открытии файла\n");
        return ProcError;
    }

    *size_of_code = GetFileSize(filename);
    *code = (int *) calloc((size_t)*size_of_code, sizeof(int));

    if (*code == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return ProcError;
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
    Stack_t stk_return = {}; // инициализация стека адресов возврата
    VerifyStackCtor(&stk, SIZE_OF_STACK);
    VerifyStackCtor(&stk_return, SIZE_OF_STACK);

    proc->stk = stk; // запись элементов в структуру процессора
    proc->stk_return = stk_return;
    proc->code = code;
    proc->size_of_code = size_of_code;
    proc->cmd_count = 0;
    
    for (int count = 0; count < NUM_OF_REGS; count++)
        proc->reg[count] = 0;

    for (int count = 0; count < SIZE_OF_RAM; count++)
        proc->RAM[count] = 0;
    
    PROC_OK(proc);
    return NoProcError;
}


int ProcErr(SPU_t * proc)
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
    if (proc->cmd_count >= proc->size_of_code)
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


int ProcDump(SPU_t * proc)
{
    if (proc == NULL)
    {
        printf("Proc [NULL POINTER!]\n");
        return 0;
    }
    printf("Proc [%p]\n", proc);
    
    StackDump(&(proc->stk));
    StackDump(&(proc->stk_return));

    if (proc->code == NULL)
    {
        printf("Code [NULL POINTER!]\n");
        return 0;
    }

    printf("\nCode [%p]: ", &(proc->code));
    for (int count = 0; count < proc->size_of_code; count++)
    {
        printf("[%d]  ", *(proc->code + count));
        if (count == proc->cmd_count)
        {
            printf("  << here  ");
        }
    }

    printf("\nSize of code: %d", proc->size_of_code);
    printf("\nCommand counter: %d\n", proc->cmd_count);
    printf("Regs [%p]: ", proc->reg);
    for (int count = 0; count < NUM_OF_REGS; count++)
    {
        printf("[%d]  ", (proc->reg)[count]);
    }

    printf("\nRAM:");
    for (int count = 0; count < SIZE_OF_RAM; count++)
    {
        printf("[%d]  ", (proc->RAM)[count]);
    }
    
    return 0;
}


ProcErr_t Calc(SPU_t * proc)
{
    PROC_OK(proc);

    bool if_end_of_calc = false;
    while (if_end_of_calc != true)
    {
        switch (proc->code[proc->cmd_count])
        {
            case PUSH:      if (PushFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case IN:        if (InFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case ADD:       if (AddFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case SUB:       if (SubFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case MUL:       if (MulFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case DIV:       if (DivFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case SQRT:      if (SqrtFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case POW:       if (PowFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case OUT:       if (OutFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case HLT:       if (HltFunc(proc, &if_end_of_calc) != NoProcError)
                                return ProcError;
                            break;

            case PUSHREG:   if (PushregFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case POPREG:    if (PopregFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case JB:        if (JbFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case JBE:       if (JbeFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case JA:        if (JaFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case JAE:       if (JaeFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case JE:        if (JeFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case JNE:       if (JneFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case CALL:      if (CallFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case RET:       if (RetFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case PUSHM:     if (PushmFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case POPM:      if (PopmFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            default:        printf("Invalid command\n");
                            ProcDtor(proc);
                                return ProcError;
        }

        proc->cmd_count++;
        PROC_OK(proc);
        /*ProcDump(proc);
        while (getchar() != '\n')
            continue;*/
    }

    return NoProcError;
}


ProcErr_t PushFunc (SPU_t * proc)
{
    assert(proc);
    proc->cmd_count++;
    VerifyStackPush(&(proc->stk), (proc->code)[proc->cmd_count]);
    return NoProcError;
}


ProcErr_t InFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0;
    printf("Введите число: ");
    scanf("%d", &x);
    VerifyStackPush(&(proc->stk), x);
    return NoProcError;
}


ProcErr_t AddFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    VerifyStackPush(&(proc->stk), x + y);
    return NoProcError;
}


ProcErr_t SubFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    VerifyStackPush(&(proc->stk), y - x);
    return NoProcError;
}


ProcErr_t MulFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    VerifyStackPush(&(proc->stk), x * y);
    return NoProcError;
}


ProcErr_t DivFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &y);
    VerifyStackPop(&(proc->stk), &x);
    if (y == 0)
    {
        printf("ERROR: Division by zero\n");
        return ProcError;
    }
    VerifyStackPush(&(proc->stk), x / y);
    return NoProcError;
}


ProcErr_t SqrtFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0;
    VerifyStackPop(&(proc->stk), &x);
    if (x < 0)
    {
        printf("ERROR: Cannot SQRT negative value\n");
        return ProcError;
    }
    x = (int)sqrt((double)x);
    VerifyStackPush(&(proc->stk), x);
    return NoProcError;
}


ProcErr_t PowFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0;
    VerifyStackPop(&(proc->stk), &x);
    proc->cmd_count++;
    int pow = (proc->code)[proc->cmd_count];
    int start_x = x;
    for (int count = 1; count < pow; count++)
        x *= start_x;
    VerifyStackPush(&(proc->stk), x);
    return NoProcError;
}


ProcErr_t OutFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0;
    VerifyStackPop(&(proc->stk), &x);
    printf("Answer: %d\n", x);
    return NoProcError;
}


ProcErr_t HltFunc (SPU_t * proc, bool * if_end_of_calc)
{
    assert(proc);
    *if_end_of_calc = true;
    return NoProcError;
}


ProcErr_t PushregFunc (SPU_t * proc)
{
    assert(proc);
    proc->cmd_count++;
    int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII;
    VerifyStackPush(&(proc->stk), (proc->reg)[num_of_reg]);
    return NoProcError;
}


ProcErr_t PopregFunc (SPU_t * proc)
{   
    assert(proc);
    int x = 0;
    VerifyStackPop(&(proc->stk), &x);
    proc->cmd_count++;
    int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII;
    (proc->reg)[num_of_reg] = x;
    return NoProcError;
}


ProcErr_t JbFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    if (x > y)
        proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    else
        proc->cmd_count++;
    return NoProcError;
}


ProcErr_t JbeFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    if (x >= y)
        proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    else
        proc->cmd_count++;
    return NoProcError;
}


ProcErr_t JaFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    if (x < y)
        proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    else
        proc->cmd_count++;
    return NoProcError;
}


ProcErr_t JaeFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    if (x <= y)
        proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    else
        proc->cmd_count++;
    return NoProcError;
}


ProcErr_t JeFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    if (x == y)
        proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    else
        proc->cmd_count++;
    return NoProcError;
}


ProcErr_t JneFunc (SPU_t * proc)
{
    assert(proc);
    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);
    if (x != y)
        proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    else
        proc->cmd_count++;
    return NoProcError;
}


ProcErr_t CallFunc (SPU_t * proc)
{
    assert(proc);
    VerifyStackPush(&(proc->stk_return), proc->cmd_count + 1);
    proc->cmd_count = (proc->code)[proc->cmd_count + 1];
    return NoProcError;
}


ProcErr_t RetFunc (SPU_t * proc)
{
    assert(proc);
    int ret_adress = 0;
    VerifyStackPop(&(proc->stk_return), &ret_adress);
    proc->cmd_count = ret_adress;
    return NoProcError;
}


ProcErr_t PushmFunc (SPU_t * proc)
{
    assert(proc);
    int reg = (proc->code)[proc->cmd_count + 1];
    proc->cmd_count++;
    int index = (proc->reg)[reg - A_ASCII]; 
    VerifyStackPush(&(proc->stk), (proc->RAM)[index]);
    return NoProcError;
}


ProcErr_t PopmFunc (SPU_t * proc)
{
    assert(proc);
    int reg = (proc->code)[proc->cmd_count + 1];
    proc->cmd_count++;
    int index = (proc->reg)[reg - A_ASCII];
    VerifyStackPop(&(proc->stk), &((proc->RAM)[index]));
    return NoProcError;
}


void ProcDtor (SPU_t * proc)
{
    assert(proc);
    assert(proc->code);

    free(proc->code);
    StackDtor(&(proc->stk));
    StackDtor(&(proc->stk_return));
}
