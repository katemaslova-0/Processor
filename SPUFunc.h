#ifndef SPUFUNC_H
#define SPUFUNC_H

#include "Data.h"
#include "Processor.h"

ProcErr_t ReadFileToCode (const char * filename, int ** code, int * size_of_code);
ProcErr_t InitProcessor(SPU_t * proc, int * code, int size_of_code);
int ProcErr(SPU_t * proc);
void ProcDtor (SPU_t * proc);
void OutputProcErrorCode(int sum_of_err);
int ProcDump(SPU_t * proc);
ProcErr_t Calc(SPU_t * proc);

#endif // SPUFUNC_H