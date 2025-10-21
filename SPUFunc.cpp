#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <math.h>

#include "Data.h"
#include "StackFunc.h"
#include "SPUFunc.h"
#include "Processor.h"

const int A_ASCII = 'A';

#define PROC_OK(proc)   if (ProcErr(proc) != NoProcError) { \
                            printf("ProcDump at %s:%d\n", __FILE__, __LINE__); \
                            int result = ProcErr(proc); \
                            ProcDump(proc); \
                            OutputProcErrorCode(result); \
                            ProcDtor(proc); \
                            return ProcError; \
                        }


static ProcErr_t PushFunc (SPU_t * proc);
static ProcErr_t InFunc (SPU_t * proc);
static ProcErr_t CalcFunc (SPU_t * proc, int cmd);
static ProcErr_t SqrtFunc (SPU_t * proc);
static ProcErr_t PowFunc (SPU_t * proc);
static ProcErr_t OutFunc (SPU_t * proc);
static ProcErr_t HltFunc (SPU_t * proc, bool * if_end_of_calc);
static ProcErr_t RegFunc (SPU_t * proc, int cmd);
static ProcErr_t JmpFunc (SPU_t * proc, int jmp);
static ProcErr_t CallFunc (SPU_t * proc);
static ProcErr_t RetFunc (SPU_t * proc);
static ProcErr_t PushmPopmFunc (SPU_t * proc, int cmd);
static ProcErr_t DrawFunc (SPU_t * proc);


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

    int return_of_fscanf = 0;
    int count = 0;
    for (count = 0; return_of_fscanf != EOF; count++)
        return_of_fscanf = fscanf(fp, "%d", &(*code)[count]);

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
    proc->RAM = (int *) calloc ((size_t)SIZE_OF_RAM, sizeof(int));

    if (proc->RAM == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return ProcError;
    }

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
            case ADD:
            case SUB:
            case MUL:
            case DIV:       if (CalcFunc(proc, proc->code[proc->cmd_count]) != NoProcError)
                                return ProcError;
                            break;

            case REMAINDER: if (CalcFunc(proc, REMAINDER) != NoProcError)
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

            case PUSHREG:
            case POPREG:    if (RegFunc(proc, proc->code[proc->cmd_count]) != NoProcError)
                                return ProcError;
                            break;

            case JB:
            case JBE:
            case JA:
            case JAE:
            case JE:
            case JNE:
            case JMP:       if (JmpFunc(proc, proc->code[proc->cmd_count]) != NoProcError)
                                return ProcError;
                            break;

            case CALL:      if (CallFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case RET:       if (RetFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            case PUSHM:     if (PushmPopmFunc(proc, PUSHM) != NoProcError)
                                return ProcError;
                            break;

            case POPM:      if (PushmPopmFunc(proc, POPM) != NoProcError)
                                return ProcError;
                            break;

            case DRAW:      if (DrawFunc(proc) != NoProcError)
                                return ProcError;
                            break;

            default:        printf("Invalid command\n");
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


static ProcErr_t PushFunc (SPU_t * proc)
{
    assert(proc);

    proc->cmd_count++;
    VerifyStackPush(&(proc->stk), (proc->code)[proc->cmd_count]);

    return NoProcError;
}


static ProcErr_t InFunc (SPU_t * proc)
{
    assert(proc);

    int x = 0;
    printf("Введите число: ");
    scanf("%d", &x);
    VerifyStackPush(&(proc->stk), x);

    return NoProcError;
}


static ProcErr_t CalcFunc (SPU_t * proc, int cmd)
{
    assert(proc);

    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);

    switch (cmd)
    {
        case ADD:       VerifyStackPush(&(proc->stk), x + y); break;
        case SUB:       VerifyStackPush(&(proc->stk), y - x); break;
        case MUL:       VerifyStackPush(&(proc->stk), x * y); break;
        case DIV:       {if (x == 0)
                        {
                            printf("ERROR: Division by zero\n");
                            return ProcError;
                        }
                        VerifyStackPush(&(proc->stk), y / x); break;}
        case REMAINDER: VerifyStackPush(&(proc->stk), y % x); break;
        default:        {printf("Invalid cmd at CalcFunc\n"); return ProcError;}
    }

    return NoProcError;
}


static ProcErr_t SqrtFunc (SPU_t * proc)
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


static ProcErr_t PowFunc (SPU_t * proc)
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


static ProcErr_t OutFunc (SPU_t * proc)
{
    assert(proc);

    int x = 0;
    VerifyStackPop(&(proc->stk), &x);
    printf("Answer: %d\n", x);

    return NoProcError;
}


static ProcErr_t HltFunc (SPU_t * proc, bool * if_end_of_calc)
{
    assert(proc);
    assert(if_end_of_calc);

    *if_end_of_calc = true;

    return NoProcError;
}


static ProcErr_t RegFunc (SPU_t * proc, int cmd)
{
    assert(proc);

    proc->cmd_count++;
    int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII;

    switch (cmd)
    {
        case PUSHREG:   VerifyStackPush(&(proc->stk), (proc->reg)[num_of_reg]);
                        break;
        case POPREG:    {int x = 0;
                        VerifyStackPop(&(proc->stk), &x);
                        (proc->reg)[num_of_reg] = x;
                        break;}
        default:        {printf("Invalid cmd at RegFunc\n"); return ProcError;}
    }

    return NoProcError;
}


static ProcErr_t JmpFunc (SPU_t * proc, int jmp)
{
    assert(proc);

    int next = (proc->code)[proc->cmd_count + 1];

    if (jmp == JMP)
    {
      proc->cmd_count = next;
      return NoProcError;
    }

    int x = 0, y = 0;
    VerifyStackPop(&(proc->stk), &x);
    VerifyStackPop(&(proc->stk), &y);

    switch (jmp)
    {
        case JB:  { if (x >  y) {proc->cmd_count = next;  return NoProcError;} } break;
        case JBE: { if (x >= y) {proc->cmd_count = next;  return NoProcError;} } break;
        case JA:  { if (x <  y) {proc->cmd_count = next;  return NoProcError;} } break;
        case JAE: { if (x <= y) {proc->cmd_count = next;  return NoProcError;} } break;
        case JE:  { if (x == y) {proc->cmd_count = next;  return NoProcError;} } break;
        case JNE: { if (x != y) {proc->cmd_count = next;  return NoProcError;} } break;
        default:  { printf("Invalid cmd at JmpFunc\n");   return ProcError;    }
    }

    proc->cmd_count++;
    return NoProcError;
}


static ProcErr_t CallFunc (SPU_t * proc)
{
    assert(proc);

    VerifyStackPush(&(proc->stk_return), proc->cmd_count + 1);
    proc->cmd_count = (proc->code)[proc->cmd_count + 1];

    return NoProcError;
}


static ProcErr_t RetFunc (SPU_t * proc)
{
    assert(proc);

    int ret_adress = 0;
    VerifyStackPop(&(proc->stk_return), &ret_adress);
    proc->cmd_count = ret_adress;

    return NoProcError;
}


static ProcErr_t PushmPopmFunc (SPU_t * proc, int cmd)
{
    assert(proc);

    int reg = (proc->code)[proc->cmd_count + 1];
    proc->cmd_count++;
    int index = (proc->reg)[reg - A_ASCII];
    switch (cmd)
    {
        case PUSHM: VerifyStackPush(&(proc->stk), (proc->RAM)[index]); break;
        case POPM:  VerifyStackPop(&(proc->stk), &((proc->RAM)[index])); break;
        default:    {printf("Invalid cmd at PushmPopmFunc\n"); return ProcError;}
    }

    return NoProcError;
}


static ProcErr_t DrawFunc (SPU_t * proc)
{
    assert(proc);
    assert(proc->RAM);

    for (int count = 0; count < SIZE_OF_RAM; count++)
    {
        printf("%c ", proc->RAM[count]);
        if ((count + 1) % 10 == 0)
            printf("\n");
    }

    return NoProcError;
}


void ProcDtor (SPU_t * proc)
{
    assert(proc);
    assert(proc->code);
    assert(proc->RAM);

    free(proc->code);
    free(proc->RAM);
    StackDtor(&(proc->stk));
    StackDtor(&(proc->stk_return));
}
