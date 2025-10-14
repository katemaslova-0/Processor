#ifndef TRANSLATEFUNC_H
#define TRANSLATEFUNC_H

int CountLines (char * buffer); // считает количество строк
void ReadFile (char * buffer, int * num_of_lines, const char * filename, int size_of_buffer); // считывание файла в буфер
void ReworkBuffer (char * buffer, char sym_to_find, char sym_to_put_instead, int size_of_buffer); /* замена в буфере всех символов
одного вида на символы другого вида */
void FillPointBuff (char * buffer, int num_of_lines, char ** text); // заполняет массив указателей
void OutputToFile (int * code, const char * output_filename, int num_of_el); // выводит строки из буфера указателей в файл
void InitData(int * size_of_buffer, char ** buffer); // инициализация буфера с текстовыми командами
StackErr_t TranslateCommands(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels); // перевод текстовых комманд в байт-код
StackErr_t CompileTwice(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels); // дважды запускает перевод текстовых команд в байт-код
void CleanAll (char ** buffer, char *** textcode, int ** code); // очистка буферов

#endif // TRANSLATEFUNC_H