#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "Data.h"
#include "TranslateFunc.h"

#define TRANSLATOR_ERROR {printf("Error at %s:%d\n", __FILE__, __LINE__); return ERROR;}

void ReadFile (char * buffer, int * num_of_lines, const char * filename, int size_of_buffer)
{
    assert(buffer != NULL);
    assert(num_of_lines != NULL);
    assert(filename != NULL);

    FILE * fp = fopen(filename, "r");

    fread(buffer, sizeof(char), (size_t)size_of_buffer, fp);
    assert(ferror(fp) == 0);

    *num_of_lines = CountLines(buffer);

    ReworkBuffer(buffer, '\n', '\0', size_of_buffer);
    fclose(fp);
}


void FillPointBuff (char * buffer, int num_of_lines, char ** text)
{
    assert(buffer != NULL);
    assert(text != NULL);

    int count = 0;
    char * ptr_buffer = buffer;
    text[count] = buffer;
   
    for (count = 1; count < num_of_lines; count++)
    {
        while (*ptr_buffer != '\0')
        {
            ptr_buffer++;
        }
        ptr_buffer++;
        text[count] = ptr_buffer; 
    }
}


void ReworkBuffer (char * buffer, char sym_to_find, char sym_to_put_instead, int size_of_buffer)
{
    assert(buffer != NULL);

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
    assert(buffer != NULL);

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
    assert(code != NULL);
    assert(output_filename != NULL);

    FILE * fp_out = fopen(output_filename, "w");

    for (int count = 0; count < num_of_el; count++)
    {
        fprintf(fp_out, "%d ", code[count]);
    }

    fclose(fp_out);
}


void InitData(int * size_of_buffer, char ** buffer)
{
    assert(size_of_buffer);
    assert(buffer);

    struct stat st;
    stat("calc.txt", &st);
    *size_of_buffer = (int)st.st_size;

    *buffer = (char *) calloc((size_t)(*size_of_buffer) + 1, sizeof(char));
    assert(buffer);
}


StackErr_t CompileTwice(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels)
{
    if (TranslateCommands(code, pos, textcode, num_of_lines, labels) != NoError) // перевод текстовых команд в байт-код(1)
    {
        return ERROR;
    }
    *pos = 0;
    if (TranslateCommands(code, pos, textcode, num_of_lines, labels) != NoError) // перевод текстовых команд в байт-код(2)
    {
        return ERROR;
    }

    return NoError;
}


StackErr_t TranslateCommands(int ** code, int * pos, char *** textcode, int num_of_lines, int * labels)
{
    assert(code);
    assert(pos);
    assert(textcode);

    int textcode_pos = 0;

    while (textcode_pos < num_of_lines)
    {
        char cmdStr[SIZE_OF_CMD_STR] = "";
        sscanf((*textcode)[textcode_pos], "%s", cmdStr);

        if (strcmp(cmdStr, "PUSH") == 0)
        {
            (*code)[(*pos)++] = PUSH;
            int arg = 0;
            sscanf((*textcode)[textcode_pos++], "PUSH %d", &arg);
            (*code)[(*pos)++] = arg;
        }
        else if (strcmp(cmdStr, "ADD") == 0)
        {
            (*code)[(*pos)++] = ADD;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "SUB") == 0)
        {
            (*code)[(*pos)++] = SUB;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "MUL") == 0)
        {
            (*code)[(*pos)++] = MUL;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "DIV") == 0)
        {
            (*code)[(*pos)++] = DIV;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "IN") == 0)
        {
            (*code)[(*pos)++] = IN;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "SQRT") == 0)
        {
            (*code)[(*pos)++] = SQRT;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "POW") == 0)
        {
            (*code)[(*pos)++] = POW;
            int arg = 0;
            sscanf((*textcode)[textcode_pos++], "POW %d", &arg);
            (*code)[(*pos)++] = arg;
        }
        else if (strcmp(cmdStr, "PUSHREG") == 0)
        {
            (*code)[(*pos)++] = PUSHREG;
            char arg = '\0';
            sscanf((*textcode)[textcode_pos++], "PUSHREG %c", &arg);
            (*code)[(*pos)++] = (int)arg;
        }
        else if (strcmp(cmdStr, "POPREG") == 0)
        {
            (*code)[(*pos)++] = POPREG;
            char arg = '\0';
            sscanf((*textcode)[textcode_pos++], "POPREG %c", &arg);
            (*code)[(*pos)++] = (int)arg;
        }
        else if (strcmp(cmdStr, "OUT") == 0)
        {
            (*code)[(*pos)++] = OUT;
            textcode_pos++;
        }
        else if (strcmp(cmdStr, "JB") == 0)
        {
            (*code)[(*pos)++] = JB;
            int num_of_label = 0;
            sscanf((*textcode)[textcode_pos++], "JB :%d", &num_of_label);
            (*code)[(*pos)++] = labels[num_of_label];
        }
        else if (strcmp(cmdStr, "JBE") == 0)
        {
            (*code)[(*pos)++] = JBE;
            int arg = 0;
            sscanf((*textcode)[textcode_pos++], "JBE :%d", &arg);
            (*code)[(*pos)++] = arg;
        }
        else if (strcmp(cmdStr, "JA") == 0)
        {
            (*code)[(*pos)++] = JA;
            int arg = 0;
            sscanf((*textcode)[textcode_pos++], "JA :%d", &arg);
            (*code)[(*pos)++] = arg;
        }
        else if (strcmp(cmdStr, "JAE") == 0)
        {
            (*code)[(*pos)++] = JAE;
            int arg = 0;
            sscanf((*textcode)[textcode_pos++], "JAE :%d", &arg);
            (*code)[(*pos)++] = arg;
        }
        else if (strcmp(cmdStr, "JE") == 0)
        {
            (*code)[(*pos)++] = JE;
            int num_of_label = 0;
            sscanf((*textcode)[textcode_pos++], "JE :%d", &num_of_label);
            (*code)[(*pos)++] = labels[num_of_label];
        }
        else if (strcmp(cmdStr, "JNE") == 0)
        {
            (*code)[(*pos)++] = JNE;
            int arg = 0;
            sscanf((*textcode)[textcode_pos++], "JNE :%d", &arg);
            (*code)[(*pos)++] = arg;
        }
        else if (strcmp(cmdStr, "HLT") == 0)
        {
            (*code)[(*pos)++] = HLT;
            textcode_pos++;
        }
        else if (strchr(cmdStr, ':') != NULL)
        {
            int num_of_label = 0;
            sscanf((*textcode)[textcode_pos++], ":%d", &num_of_label);
            labels[num_of_label] = *pos - 1;
        }
        else
            TRANSLATOR_ERROR;
    }

    return NoError;
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