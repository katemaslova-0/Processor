#ifndef TRANSLATEFUNC_H
#define TRANSLATEFUNC_H

const int QUANTITY_OF_COMMANDS = 25;

enum TranslErr_t
{
    NoTranslError       = (1 << 0),
    TranslError         = (1 << 1)
};

enum Arg_t
{
    NoArg   = 0,
    IntArg  = 1,
    CharArg = 2 // reg arg
};

struct Cmd_t
{
    const char * name;
    int num_of_cmd;
    int quant_of_arg;
    Arg_t type_of_arg;
};

int CountLines (char * buffer); // считает количество строк
TranslErr_t ReadFile (char * buffer, int * num_of_lines, const char * filename, int size_of_buffer); // считывание файла в буфер
void ReworkBuffer (char * buffer, char sym_to_find, char sym_to_put_instead, int size_of_buffer); /* замена в буфере всех символов
одного вида на символы другого вида */
TranslErr_t FillPointBuff (char * buffer, int num_of_lines, char *** text); // заполняет массив указателей
TranslErr_t OutputToFile (int * code, const char * output_filename, int num_of_el); // выводит строки из буфера указателей в файл
TranslErr_t InitData(int * size_of_buffer, char ** buffer, const char * filename); // инициализация буфера с текстовыми командами
TranslErr_t TranslateCommands(int ** code, int * pos, char ** textcode, int num_of_lines, int * labels); // перевод текстовых комманд в байт-код
TranslErr_t CompileTwice(int ** code, int * pos, char ** textcode, int num_of_lines, int * labels); // дважды запускает перевод текстовых команд в байт-код
void CleanAll (char ** buffer, char *** textcode, int ** code); // очистка буферов
TranslErr_t ReadArg (int * arg, char * str, int * labels, int count); // считывает аргументы команд
int GetFileSize (const char * filename); // получение размера файла
void TranslateLabel (int pos, char ** textcode, int * labels, int textcode_pos); // обрабатывает метки
int GetCommandNumber (char * cmdStr); // получение номера команды
void SkipSpaces (char ** str); // пропуск пробелов, табуляции и т.п.
int ReadLabel (char * str, int * labels); // считывание метки
TranslErr_t ReadRamArg (char ** str, int * arg, int count); // считывание аргумента оперативной памяти
TranslErr_t ReadCharArg(char * str, int * arg, int count); // считывание имени регистра
TranslErr_t ReadIntArg (char * str, int * arg, int count); // считывание константы

#endif // TRANSLATEFUNC_H
