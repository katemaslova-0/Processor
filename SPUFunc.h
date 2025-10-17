#ifndef SPUFUNC_H
#define SPUFUNC_H

#include "Data.h"
#include "Processor.h"

ProcErr_t ReadFileToCode (const char * filename, int ** code, int * size_of_code); // считывает байт-код из файла в массив
ProcErr_t InitProcessor(SPU_t * proc, int * code, int size_of_code); // инициализирует процессор
int ProcErr(SPU_t * proc); // верификация
void ProcDtor (SPU_t * proc); // очистка буферов
void OutputProcErrorCode(int sum_of_err); // вывод кодов ошибок с пояснениями
int ProcDump(SPU_t * proc); // DUMP
ProcErr_t Calc(SPU_t * proc); // исполнение команд
int GetFileSize (const char * filename); // получение размера файла

ProcErr_t PushFunc (SPU_t * proc);
ProcErr_t InFunc (SPU_t * proc);
ProcErr_t AddFunc (SPU_t * proc);
ProcErr_t SubFunc (SPU_t * proc);
ProcErr_t MulFunc (SPU_t * proc);
ProcErr_t DivFunc (SPU_t * proc);
ProcErr_t SqrtFunc (SPU_t * proc);
ProcErr_t PowFunc (SPU_t * proc);
ProcErr_t OutFunc (SPU_t * proc);
ProcErr_t HltFunc (SPU_t * proc, bool * if_end_of_calc);
ProcErr_t PushregFunc (SPU_t * proc);
ProcErr_t PopregFunc (SPU_t * proc);
ProcErr_t JbFunc (SPU_t * proc);
ProcErr_t JbeFunc (SPU_t * proc);
ProcErr_t JaFunc (SPU_t * proc);
ProcErr_t JaeFunc (SPU_t * proc);
ProcErr_t JeFunc (SPU_t * proc);
ProcErr_t JneFunc (SPU_t * proc);
ProcErr_t CallFunc (SPU_t * proc);
ProcErr_t RetFunc (SPU_t * proc);
ProcErr_t PushmFunc (SPU_t * proc);
ProcErr_t PopmFunc (SPU_t * proc);

#endif // SPUFUNC_H