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
                        return PROC_ERROR; \
                        }

#define DO_PUSH     {proc->cmd_count++; VerifyStackPush(&(proc->stk), (proc->code)[proc->cmd_count]); break;}
#define DO_IN       {int x = 0; printf("Введите число: "); scanf("%d", &x); VerifyStackPush(&(proc->stk), x); break;}
#define DO_ADD      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    VerifyStackPush(&(proc->stk), x + y); break;}
#define DO_SUB      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    VerifyStackPush(&(proc->stk), y - x); break;}
#define DO_MUL      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    VerifyStackPush(&(proc->stk), x * y); break;}
#define DO_DIV      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &y); VerifyStackPop(&(proc->stk), &x); \
                    if (y == 0) { printf("ERROR: Division by zero\n"); return PROC_ERROR;} \
                    VerifyStackPush(&(proc->stk), x / y); break;}
#define DO_SQRT     {int x = 0; VerifyStackPop(&(proc->stk), &x); \
                    if (x < 0) {printf("ERROR: Cannot SQRT negative value\n"); return PROC_ERROR;} \
                    x = (int)sqrt((double)x); VerifyStackPush(&(proc->stk), x); break;}
#define DO_POW      {int x = 0; VerifyStackPop(&(proc->stk), &x); proc->cmd_count++; \
                    int pow = (proc->code)[proc->cmd_count]; int start_x = x; \
                    for (int count = 1; count < pow; count++) {x *= start_x;} \
                    VerifyStackPush(&(proc->stk), x); break;}
#define DO_OUT      {int x = 0; VerifyStackPop(&(proc->stk), &x); printf("Answer: %d\n", x); break;}
#define DO_HLT      {if_end_of_calc = true; break;}
#define DO_PUSHREG  {proc->cmd_count++; int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII;\
                    VerifyStackPush(&(proc->stk), (proc->reg)[num_of_reg]); break;}
#define DO_POPREG   {int x = 0; VerifyStackPop(&(proc->stk), &x); proc->cmd_count++; \
                    int num_of_reg = (proc->code)[proc->cmd_count] - A_ASCII; (proc->reg)[num_of_reg] = x; break;}
#define DO_JB       {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    if (x > y) {proc->cmd_count = (proc->code)[proc->cmd_count + 1];} else {proc->cmd_count++;} break;}
#define DO_JBE      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    if (x >= y) {proc->cmd_count = (proc->code)[proc->cmd_count + 1];} else {proc->cmd_count++;} break;}
#define DO_JA       {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    if (x < y) {proc->cmd_count = (proc->code)[proc->cmd_count + 1];} else {proc->cmd_count++;} break;}
#define DO_JAE      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    if (x <= y) {proc->cmd_count = (proc->code)[proc->cmd_count + 1];} else {proc->cmd_count++;} break;}
#define DO_JE       {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    if (x == y) {proc->cmd_count = (proc->code)[proc->cmd_count + 1];} else {proc->cmd_count++;} break;}
#define DO_JNE      {int x = 0, y = 0; VerifyStackPop(&(proc->stk), &x); VerifyStackPop(&(proc->stk), &y); \
                    if (x != y) {proc->cmd_count = (proc->code)[proc->cmd_count + 1];} else {proc->cmd_count++;} break;}
#define DO_CALL     {VerifyStackPush(&(proc->stk_return), proc->cmd_count + 1); \
                    proc->cmd_count = (proc->code)[proc->cmd_count + 1]; break;}
#define DO_RET      {int ret_adress = 0; VerifyStackPop(&(proc->stk_return), &ret_adress); \
                    proc->cmd_count = ret_adress; break;}
#define DO_PUSHM    {int reg = (proc->code)[proc->cmd_count + 1]; int index = (proc->reg)[reg - A_ASCII]; \
                    VerifyStackPush(&(proc->stk), (proc->RAM)[index]); break;}
#define DO_POPM     {int reg = (proc->code)[proc->cmd_count + 1]; int index = (proc->reg)[reg - A_ASCII]; \
                    VerifyStackPop(&(proc->stk), &((proc->RAM)[index])); break;}
#define INV_CMD     {printf("Invalid command\n"); ProcDtor(proc); return PROC_ERROR;}


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
            case PUSH:      DO_PUSH;
            case IN:        DO_IN;
            case ADD:       DO_ADD;
            case SUB:       DO_SUB;
            case MUL:       DO_MUL;
            case DIV:       DO_DIV;
            case SQRT:      DO_SQRT;
            case POW:       DO_POW;
            case OUT:       DO_OUT;
            case HLT:       DO_HLT;
            case PUSHREG:   DO_PUSHREG;
            case POPREG:    DO_POPREG;
            case JB:        DO_JB;
            case JBE:       DO_JBE;
            case JA:        DO_JA;
            case JAE:       DO_JAE;
            case JE:        DO_JE;
            case JNE:       DO_JNE;
            case CALL:      DO_CALL;
            case RET:       DO_RET;
            case PUSHM:     DO_PUSHM;
            case POPM:      DO_POPM;
            default:        INV_CMD;
        }

        proc->cmd_count++;
        PROC_OK(proc);
        /*ProcDump(proc);
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
    StackDtor(&(proc->stk_return));
}
