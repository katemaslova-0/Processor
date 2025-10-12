#ifndef SPUFUNC_H
#define SPUFUNC_H

#include "Data.h"
#include "Processor.h"

const int A_ASCII = 65;

#define PROC_OK(proc, size_of_code) if (ProcErr(proc, size_of_code) != NoProcError) { \
                                    printf("ProcDump at %s:%d\n", __FILE__, __LINE__); \
                                    int result = ProcErr(proc, size_of_code); \
                                    ProcDump(proc, size_of_code); \
                                    OutputProcErrorCode(result); \
                                    ProcDtor(proc); \
                                    return PROC_ERROR; \
                                    }

ProcErr_t ReadFileToCode (const char * filename, int ** code, int * size_of_code);
ProcErr_t InitProcessor(SPU_t * proc, int * code, int size_of_code);
int ProcErr(SPU_t * proc, int size_of_code);
void ProcDtor (SPU_t * proc);
void OutputProcErrorCode(int sum_of_err);
int ProcDump(SPU_t * proc, int size_of_code);
ProcErr_t Calc(SPU_t * proc, int size_of_code);

#endif // SPUFUNC_H