#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "Data.h"
#include "StackFunc.h"
#include "SPUFunc.h"
#include "Processor.h"

int main (void)
{
    int * code = NULL; // инициализация указателя на буфер
    int size_of_code = 0; // инициализация размера буфера
    const char * filename = "translator_out.txt"; // инициализация строки с именем файла с байт-кодом
    
    if (ReadFileToCode(filename, &code, &size_of_code) != NoProcError) // считывание файла в буфер
    {
        //
        return -1;
    }

    SPU_t proc1 = {};
    if (InitProcessor(&proc1, code, size_of_code) != NoProcError) // инициализация процессора
    {
        ProcDtor(&proc1);
        return -1;
    }
    if (Calc(&proc1, size_of_code) != NoProcError) // выполнение команд
    {
        ProcDtor(&proc1);
        return -1;
    }

    ProcDtor(&proc1); // очистка буферов

    return 0;
}