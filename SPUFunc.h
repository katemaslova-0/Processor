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

#endif // SPUFUNC_H