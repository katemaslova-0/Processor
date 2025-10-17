#ifndef TRANSLATEFUNC_H
#define TRANSLATEFUNC_H

const int QUANTITY_OF_COMMANDS = 23;

enum TranslErr_t
{
    NoTranslError       = (1 << 0),
    TranslError         = (1 << 1)
};

struct Cmd_t
{
    const char * name;
    int num_of_cmd;
    int quant_of_arg;
};

int CountLines (char * buffer); // считает количество строк
void ReadFile (char * buffer, int * num_of_lines, const char * filename, int size_of_buffer); // считывание файла в буфер
void ReworkBuffer (char * buffer, char sym_to_find, char sym_to_put_instead, int size_of_buffer); /* замена в буфере всех символов
одного вида на символы другого вида */
TranslErr_t FillPointBuff (char * buffer, int num_of_lines, char *** text); // заполняет массив указателей
void OutputToFile (int * code, const char * output_filename, int num_of_el); // выводит строки из буфера указателей в файл
void InitData(int * size_of_buffer, char ** buffer, const char * filename); // инициализация буфера с текстовыми командами
TranslErr_t TranslateCommands(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels, Cmd_t * commands_buf); // перевод текстовых комманд в байт-код
TranslErr_t CompileTwice(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels, Cmd_t * commands_buf); // дважды запускает перевод текстовых команд в байт-код
void CleanAll (char ** buffer, char *** textcode, int ** code, Cmd_t ** commands_buf); // очистка буферов
int ReadArg (char * str, int * labels); // считывает аргументы команд
TranslErr_t FillStructBuff (Cmd_t ** commands_buf); // заполняет массив структур для команд
int GetFileSize (const char * filename); // получение размера файла

#endif // TRANSLATEFUNC_H