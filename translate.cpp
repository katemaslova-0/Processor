#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <string.h>

#include "Data.h"
#include "TranslateFunc.h"

const int NUM_OF_LABELS = 10;

int main (void)
{
    int num_of_lines = 0;
    int size_of_buffer = 0;
    char * buffer = NULL;
    int labels[NUM_OF_LABELS] = {};
    const char * filename = "testcall.txt";
    const char * output_filename = "translator_out.txt";
    
    InitData(&size_of_buffer, &buffer); // инициализация буфера с текстовыми командами
    ReadFile (buffer, &num_of_lines, filename, size_of_buffer); // считывание файла в буфер
    char ** textcode = (char **) calloc((size_t)num_of_lines, sizeof(char *)); // заполнение массива указателей
    assert(textcode);
    FillPointBuff(buffer, num_of_lines, textcode); // return

    int * code = (int *) calloc ((size_t)(num_of_lines * 2), sizeof(int)); // инициализация массива с байт-кодом
    assert(code);
    int pos = 0; // инициализация счётчика количества элементов

    if (CompileTwice(&code, &pos, &textcode, num_of_lines, labels) != NoError)
    {
        CleanAll(&buffer, &textcode, &code);
        return -1;
    }

    OutputToFile (code, output_filename, pos); // запись байт-кода в файл
    CleanAll(&buffer, &textcode, &code); // очистка буферов

    return 0;
}
