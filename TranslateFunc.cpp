#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

#include "Data.h"
#include "TranslateFunc.h"

const int SIZE_OF_CMD_STR = 40;

#define TRANSLATOR_ERROR {printf("Error at %s:%d\n", __FILE__, __LINE__); return TranslError;}

// Даня Жебряков: добавлен static const, перенесён из translate.cpp
static const Cmd_t commands_buf[QUANTITY_OF_COMMANDS] =   { {"PUSH",        PUSH,      1, IntArg}, // enum for type of arg
                                                            {"ADD",         ADD,       0, NoArg}, 
                                                            {"SUB",         SUB,       0, NoArg},
                                                            {"MUL",         MUL,       0, NoArg},
                                                            {"DIV",         DIV,       0, NoArg},
                                                            {"POW",         POW,       1, IntArg},
                                                            {"REMAINDER",   REMAINDER, 0, NoArg},
                                                            {"OUT",         OUT,       0, NoArg},
                                                            {"HLT",         HLT,       0, NoArg},
                                                            {"PUSHREG",     PUSHREG,   1, CharArg},
                                                            {"POPREG",      POPREG,    1, CharArg},
                                                            {"JB",          JB,        1, IntArg},
                                                            {"JBE",         JBE,       1, IntArg},
                                                            {"JA",          JA,        1, IntArg},
                                                            {"JAE",         JAE,       1, IntArg},
                                                            {"JE",          JE,        1, IntArg},
                                                            {"JNE",         JNE,       1, IntArg},
                                                            {"JMP",         JMP,       1, IntArg},
                                                            {"SQRT",        SQRT,      0, NoArg},
                                                            {"IN",          IN,        0, NoArg},
                                                            {"CALL",        CALL,      1, IntArg},
                                                            {"RET",         RET,       0, NoArg},
                                                            {"PUSHM",       PUSHM,     1, CharArg},
                                                            {"POPM",        POPM,      1, CharArg}, 
                                                            {"DRAW",        DRAW,      0, NoArg}};

void ReadFile (char * buffer, int * num_of_lines, const char * filename, int size_of_buffer)
{
    assert(buffer);
    assert(num_of_lines);
    assert(filename);

    FILE * fp = fopen(filename, "r");
    assert(fp);

    fread(buffer, sizeof(char), (size_t)size_of_buffer, fp);
    assert(ferror(fp) == 0);

    *num_of_lines = CountLines(buffer);

    ReworkBuffer(buffer, '\n', '\0', size_of_buffer);
    fclose(fp);
}


TranslErr_t FillPointBuff (char * buffer, int num_of_lines, char *** text)
{
    assert(buffer);
    assert(text);

    int count = 0;
    *text = (char **) calloc((size_t)num_of_lines, sizeof(char *));
    if (*text == NULL)
    {
        printf("Ошибка выделения памяти\n"); // Даня Жебряков: вынести в main, можно заменить на perror
        return TranslError;
    }
    char * ptr_buffer = buffer;
    (*text)[count] = buffer;
   
    for (count = 1; count < num_of_lines; count++)
    {
        while (*ptr_buffer != '\0')
        {
            ptr_buffer++;
        }
        ptr_buffer++;
        (*text)[count] = ptr_buffer; 
    }

    return NoTranslError;
}


void ReworkBuffer (char * buffer, char sym_to_find, char sym_to_put_instead, int size_of_buffer)
{
    assert(buffer);

    int count = 0;
    while (count < size_of_buffer - 1)
    {
        if (*(buffer + count) == sym_to_find)
        {
            *(buffer + count) = sym_to_put_instead;
        }
        count++;
    }

    *(buffer + count) = '\0';
}


int CountLines (char * buffer)
{
    assert(buffer);

    int num_of_lines = 0;
    int count = 0;
    while(*(buffer + count) != '\0')
    {
        if (*(buffer + count) == '\n')
        {
            num_of_lines++;
        }
        count++;
    }

    return num_of_lines;
}


void OutputToFile (int * code, const char * output_filename, int num_of_el)
{  
    assert(code);
    assert(output_filename);

    FILE * fp_out = fopen(output_filename, "w");
    assert(fp_out);

    for (int count = 0; count < num_of_el; count++)
    {
        fprintf(fp_out, "%d ", code[count]);
    }

    fclose(fp_out);
}


TranslErr_t InitData(int * size_of_buffer, char ** buffer, const char * filename)
{
    assert(size_of_buffer);
    assert(buffer);
    assert(filename);

    *size_of_buffer = GetFileSize(filename);

    *buffer = (char *) calloc((size_t)(*size_of_buffer) + 1, sizeof(char));
    if (*buffer == NULL)
    {
        printf("Ошибка выделения памяти");
        return TranslError;
    }

    return NoTranslError;
}


TranslErr_t CompileTwice(int ** code, int * pos, char ** textcode, int num_of_lines, int * labels)
{
    assert(code);
    assert(pos);
    assert(textcode);
    assert(labels);

    *code = (int *) calloc ((size_t)(num_of_lines * 2), sizeof(int));
    if (*code == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return TranslError;
    }

    if (TranslateCommands(code, pos, textcode, num_of_lines, labels) != NoTranslError)
    {
        return TranslError;
    }
    *pos = 0;
    if (TranslateCommands(code, pos, textcode, num_of_lines, labels) != NoTranslError)
    {
        return TranslError;
    }

    return NoTranslError;
}


int ReadArg (char * str, int * labels, int count)
{
    assert(str);
    assert(labels);

    int arg_d = 0;
    char arg_c = '\0';

    while (*str != ' ')
        str++;
    str++;

    if (*str == ':')
    {
        str++;
        int num_of_label = 0;
        sscanf(str, "%d", &num_of_label);
        return labels[num_of_label];
    }
    else if (*str == '[')
    {
        str++;
    }

    if (isalpha(*str) != 0)
    {
        if ((commands_buf[count]).type_of_arg != CharArg)
        {
            printf("Ошибка в типе аргумента команды\n");
            //return TranslError;
        }
        sscanf(str, "%c", &arg_c);
        return (int)arg_c;
    }
    else
    {
        if ((commands_buf[count]).type_of_arg != IntArg)
        {
            printf("Ошибка в типе аргумента команды\n");
            return TranslError;
        }
        sscanf(str, "%d", &arg_d);
        return arg_d;
    }
}


TranslErr_t TranslateCommands (int ** code, int * pos, char ** textcode, int num_of_lines, int * labels)
{
    assert(code);
    assert(pos);
    assert(textcode);
    assert(labels);

    int textcode_pos = 0;

    while (textcode_pos < num_of_lines)
    {
        bool if_cmd_found = false;
        char cmdStr[SIZE_OF_CMD_STR] = "";
        sscanf((textcode)[textcode_pos], "%s", cmdStr);

        for (int count = 0; count < QUANTITY_OF_COMMANDS; count++)
        {
            if (strcmp(cmdStr, (&(commands_buf[count]))->name) == 0)
            {
                (*code)[(*pos)++] = count;
                // Danila Zhebryakov: eliminated redundant else
                if ((&(commands_buf[count]))->quant_of_arg == 1)
                {
                    int arg = ReadArg((textcode)[textcode_pos], labels, count);
                    (*code)[(*pos)++] = arg;
                }
                textcode_pos++;
                if_cmd_found = true;
                break;
            }
        }
        if (if_cmd_found == false)
        {
            if (strchr(cmdStr, ':') != NULL)
            {
                int num_of_label = 0;
                sscanf((textcode)[textcode_pos++], ":%d", &num_of_label);
                labels[num_of_label] = *pos - 1;
            }
            else if (cmdStr[0] == '\0') // DanilaZhebryakov: fix improper use of strchr
                textcode_pos++;
            else
                TRANSLATOR_ERROR;
        }
    
    }

    return NoTranslError;
}


int GetFileSize (const char * filename)
{
    assert(filename);

    struct stat st;
    stat(filename, &st);
    return (int)st.st_size;
}


void CleanAll (char ** buffer, char *** textcode, int ** code)
{
    assert(buffer);
    assert(textcode);
    assert(code);

    free(*buffer);
    free(*textcode);
    free(*code);
}