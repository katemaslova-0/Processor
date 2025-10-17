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


void InitData(int * size_of_buffer, char ** buffer, const char * filename)
{
    assert(size_of_buffer);
    assert(buffer);
    assert(filename);

    *size_of_buffer = GetFileSize(filename);

    *buffer = (char *) calloc((size_t)(*size_of_buffer) + 1, sizeof(char));
    assert(buffer);
}


TranslErr_t CompileTwice(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels, Cmd_t * commands_buf)
{
    assert(code);
    assert(pos);
    assert(textcode);
    assert(labels);
    assert(commands_buf);

    *code = (int *) calloc ((size_t)(num_of_lines * 2), sizeof(int));
    if (*code == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return TranslError;
    }

    if (TranslateCommands(code, pos, textcode, num_of_lines, labels, commands_buf) != NoTranslError)
    {
        return TranslError;
    }
    *pos = 0;
    if (TranslateCommands(code, pos, textcode, num_of_lines, labels, commands_buf) != NoTranslError)
    {
        return TranslError;
    }

    return NoTranslError;
}


int ReadArg (char * str, int * labels)
{
    assert(str);
    assert(labels);

    int arg = 0;
    char arg_c = '\0';

    while (*str != ' ')
        str++;
    str++;

    if (*str == '[')
        str++;
    else if (*str == ':')
    {
        str++;
        int num_of_label = 0;
        sscanf(str, "%d", &num_of_label);
        return labels[num_of_label];
    }

    if (isalpha(*str) != 0)
    {
        sscanf(str, "%c", &arg_c);
        return (int)arg_c;
    }
    else
    {
        sscanf(str, "%d", &arg);
        return arg;
    }
}


TranslErr_t TranslateCommands(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels, Cmd_t * commands_buf)
{
    assert(code);
    assert(pos);
    assert(textcode);
    assert(labels);
    assert(commands_buf);

    int textcode_pos = 0;

    while (textcode_pos < num_of_lines)
    {
        bool if_cmd_found = false;
        char cmdStr[SIZE_OF_CMD_STR] = "";
        sscanf((*textcode)[textcode_pos], "%s", cmdStr);

        for (int count = 0; count < QUANTITY_OF_COMMANDS; count++)
        {
            if (strcmp(cmdStr, (&(commands_buf[count]))->name) == 0)
            {
                if ((&(commands_buf[count]))->quant_of_arg == 0)
                {
                    (*code)[(*pos)++] = count; // how to use enum?
                    textcode_pos++;
                }
                else
                {
                    (*code)[(*pos)++] = count;
                    int arg = ReadArg((*textcode)[textcode_pos++], labels);
                    (*code)[(*pos)++] = arg;
                }
                if_cmd_found = true;
                break;
            }
        }
        if (if_cmd_found == false)
        {
            if (strchr(cmdStr, ':') != NULL)
            {
                int num_of_label = 0;
                sscanf((*textcode)[textcode_pos++], ":%d", &num_of_label);
                labels[num_of_label] = *pos - 1;
            }
            else if (strchr(cmdStr, '\0') != NULL)
                textcode_pos++;
            else
                TRANSLATOR_ERROR;
        }
    
    }

    return NoTranslError;
}


TranslErr_t FillStructBuff (Cmd_t ** commands_buf)
{
    assert(commands_buf);

    const char * names_array[QUANTITY_OF_COMMANDS] = {  "PUSH", "ADD", "SUB","MUL", "DIV",
                                                        "POW", "OUT", "HLT", "PUSHREG",
                                                        "POPREG", "JB", "JBE", "JA", "JAE",
                                                        "JE", "JNE", "SQRT", "IN", "CALL",
                                                        "RET", "PUSHM", "POPM", "DRAW"};

    *commands_buf = (Cmd_t *) calloc ((size_t)QUANTITY_OF_COMMANDS, sizeof(Cmd_t));

    if (*commands_buf == NULL)
    {
        printf("Ошибка выделения памяти\n");
        return TranslError;
    }

    for (int count = 0; count < QUANTITY_OF_COMMANDS; count++)
    {
        (&((*commands_buf)[count]))->name = names_array[count];
        (&((*commands_buf)[count]))->num_of_cmd = count;
        if ((count == ADD) || (count == SUB) || (count == MUL) || (count == DIV) || (count == OUT) ||
            (count == HLT) || (count == SQRT) || (count == IN) || (count == RET) || (count == DRAW))
        {
            (&((*commands_buf)[count]))->quant_of_arg = 0;
        }
        else
        {
            (&((*commands_buf)[count]))->quant_of_arg = 1;
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


void CleanAll (char ** buffer, char *** textcode, int ** code, Cmd_t ** commands_buf)
{
    assert(buffer);
    assert(textcode);
    assert(code);
    assert(commands_buf);

    free(*buffer);
    free(*textcode);
    free(*code);
    free(*commands_buf);
}