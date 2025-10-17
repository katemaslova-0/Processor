#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <string.h>

#include "Data.h"
#include "TranslateFunc.h"

const int NUM_OF_LABELS = 10;
const char * filename = "factorial.txt";
const char * output_filename = "translator_out.txt";

int main (void)
{
    int num_of_lines = 0;
    int size_of_buffer = 0;
    char * buffer = NULL;
    int labels[NUM_OF_LABELS] = {};
    int * code = NULL;
    char ** textcode = NULL;
    Cmd_t * commands_buf = NULL;
    
    InitData(&size_of_buffer, &buffer, filename); // инициализация буфера с текстовыми командами
    ReadFile (buffer, &num_of_lines, filename, size_of_buffer); // считывание файла в буфер

    if (FillStructBuff(&commands_buf) != NoTranslError) // заполнение массива структур с данными о командах
    {
        CleanAll(&buffer, &textcode, &code, &commands_buf);
        return -1;
    }

    if (FillPointBuff(buffer, num_of_lines, &textcode) != NoTranslError) // заполнение массива указателей на строки
    {
        CleanAll(&buffer, &textcode, &code, &commands_buf);
        return -1;
    }

    int pos = 0; // инициализация счётчика количества элементов
    if (CompileTwice(&code, &pos, &textcode, num_of_lines, labels, commands_buf) != NoTranslError)
    {
        CleanAll(&buffer, &textcode, &code, &commands_buf);
        return -1;
    }

    OutputToFile (code, output_filename, pos); // запись байт-кода в файл
    CleanAll(&buffer, &textcode, &code, &commands_buf); // очистка буферов

    return 0;
}
