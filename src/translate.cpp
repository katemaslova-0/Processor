#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <string.h>

#include "Data.h"
#include "TranslateFunc.h"

const int NUM_OF_LABELS = 10;
const char * output_filename = "translator_out.txt";

int main (int argc, char * argv[])
{
    int num_of_lines = 0;
    int size_of_buffer = 0;
    char * buffer = NULL;
    int labels[NUM_OF_LABELS] = {};
    int * code = NULL;
    char ** textcode = NULL;

    if (argc != 2)
    {
        printf("Invalid command line arguments\n");
        return -1;
    }
    const char * filename = argv[1];

    if (InitData(&size_of_buffer, &buffer, filename) != NoTranslError) // инициализация буфера с текстовыми командами
    {
        return -1;
    }

    if (ReadFile (buffer, &num_of_lines, filename, size_of_buffer) != NoTranslError) // считывание файла в буфер
    {
        CleanAll(&buffer, &textcode, &code);
        return -1;
    }

    if (FillPointBuff(buffer, num_of_lines, &textcode) != NoTranslError) // заполнение массива указателей на строки
    {
        CleanAll(&buffer, &textcode, &code);
        return -1;
    }

    int pos = 0; // инициализация счётчика количества элементов
    if (CompileTwice(&code, &pos, textcode, num_of_lines, labels) != NoTranslError)
    {
        CleanAll(&buffer, &textcode, &code);
        return -1;
    }

    if (OutputToFile(code, output_filename, pos) != NoTranslError) // запись байт-кода в файл
    {
        CleanAll(&buffer, &textcode, &code);
        return -1;
    }

    CleanAll(&buffer, &textcode, &code); // очистка буферов

    return 0;
}
