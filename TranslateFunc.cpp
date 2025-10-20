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

static const Cmd_t commands_buf[QUANTITY_OF_COMMANDS] =   { {"PUSH",        PUSH,      1, IntArg},
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

TranslErr_t ReadFile (char * buffer, int * num_of_lines, const char * filename, int size_of_buffer)
{
    assert(buffer);
    assert(num_of_lines);
    assert(filename);

    FILE * fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Ошибка открытия файла в ReadFile\n");
        return TranslError;
    }

    fread(buffer, sizeof(char), (size_t)size_of_buffer, fp);
    assert(ferror(fp) == 0);

    *num_of_lines = CountLines(buffer);

    ReworkBuffer(buffer, '\n', '\0', size_of_buffer);
    fclose(fp);
    return NoTranslError;
}


TranslErr_t FillPointBuff (char * buffer, int num_of_lines, char *** text)
{
    assert(buffer);
    assert(text);

    int count = 0;
    *text = (char **) calloc((size_t)num_of_lines, sizeof(char *));
    if (*text == NULL)
    {
        printf("Ошибка выделения памяти\n");
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


TranslErr_t OutputToFile (int * code, const char * output_filename, int num_of_el)
{
    assert(code);
    assert(output_filename);

    FILE * fp_out = fopen(output_filename, "w");
    if (fp_out == NULL)
    {
        printf("Ошибка открытия файла в OutputToFile\n");
        return TranslError;
    }

    for (int count = 0; count < num_of_el; count++)
    {
        fprintf(fp_out, "%d ", code[count]);
    }

    fclose(fp_out);
    return NoTranslError;
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


TranslErr_t ReadArg (int * arg, char * str, int * labels, int count)
{
    assert(arg);
    assert(str);
    assert(labels);

    int arg_d = 0;
    char arg_c = '\0';

    while (*str != ' ') // isspace + new func
        str++;
    str++;

    if (*str == ':')
    {
        str++;
        int num_of_label = 0;
        sscanf(str, "%d", &num_of_label);
        *arg = labels[num_of_label]; // new func
        return NoTranslError;
    }
    else if (*str == '[') // check
    {
        str++;
    }

    if (isalpha(*str) != 0)
    {
        if ((commands_buf[count]).type_of_arg != CharArg)
        {
            printf("Ошибка в типе аргумента команды\n"); // "test.asm:12: Ошибка в типе CharArg аргумента 234 команды PushReg\n"
            return TranslError;
        }
        sscanf(str, "%c", &arg_c);
        *arg = (int)arg_c;
        return NoTranslError;
    }
    else
    {
        if ((commands_buf[count]).type_of_arg != IntArg)
        {
            printf("Ошибка в типе аргумента команды\n");
            return TranslError;
        }
        sscanf(str, "%d", &arg_d);
        *arg = arg_d;
        return NoTranslError;
    }

    return NoTranslError;
}


TranslErr_t TranslateCommands (int ** code, int * pos, char ** textcode, int num_of_lines, int * labels)
{
    assert(code);
    assert(pos);
    assert(textcode);
    assert(labels);

    int textcode_pos = 0;

    // структура для аргументов!

    while (textcode_pos < num_of_lines) // for?
    {
        bool if_cmd_found = false;
        char cmdStr[SIZE_OF_CMD_STR] = "";
        sscanf((textcode)[textcode_pos], "%s", cmdStr);

        /*if (cmdStr[0] == ':')
        {
            TranslateLabel(...);
            continue;
        }

        int count = CheckCommandNumber(cmdStr);
        (*code)[(*pos)++] = count;

        if ((&(commands_buf[count]))->quant_of_arg == 1)
        {
            int arg = 0;
            if (ReadArg(&arg, (textcode)[textcode_pos], labels, count) != NoTranslError)
                return TranslError;
            (*code)[(*pos)++] = arg;
        }*/



        for (int count = 0; count < QUANTITY_OF_COMMANDS; count++)
        {
            if (strcmp(cmdStr, (&(commands_buf[count]))->name) == 0)
            {
                (*code)[(*pos)++] = count;
                if ((&(commands_buf[count]))->quant_of_arg == 1)
                {
                    int arg = 0;
                    if (ReadArg(&arg, (textcode)[textcode_pos], labels, count) != NoTranslError)
                        return TranslError;
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
            else if (cmdStr[0] == '\0')
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
