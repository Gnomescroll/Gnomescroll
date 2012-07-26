#include "files.hpp"

#include "macros.hpp"

off_t fsize(const char *filename)
{
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

// free the returned char* buffer after use
char* read_file_to_buffer(char* file_name, int* size)
{

    int expected_size = (int)fsize(file_name);
    char *source = NULL;
    FILE *fp = fopen(file_name, "r");
    if (fp != NULL)
    {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) 
        {
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            *size = (int)bufsize;
            if(*size != expected_size) printf("Warning: size of %s differs from expected\n", file_name);
            if (bufsize == -1) { /* Error */ }
            /* Allocate our buffer to that size. */
            source = (char*) calloc(bufsize+2, sizeof(char));
            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) == 0) { /* Error */ }
            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) 
            {
                free(source);
                printf("read_file_to_buffer: error reading file %s", file_name);
                return NULL;
            } 
            else 
            {
                source[++newLen] = '\0'; /* Just to be safe. */
            }
        }
        if(fp == NULL) printf("WTF\n");
        fclose(fp);
    }
    else
    {
        printf("read_file_to_buffer: error, could not open %s \n", file_name);
        GS_ABORT();
    }
    //free(source); /* Don't forget to call free() later! */

    return source;
}

int count_lines(char* buffer)
{
    int lines = 0;
    int i = 0;
    char c;
    while ((c = buffer[i++]) != '\0')
        if (c == '\n')
            lines++;
    return lines;
}

int count_lines(char* buffer, int* longest)
{
    int lines = 1;
    int length = 0;
    int i = 0;
    char c;
    while ((c = buffer[i++]) != '\0')
    {
        if (c == '\n')
        {
            if (length > *longest)
                *longest = length;
            length = 0;
            lines++;
        }
        else
            length++;
    }
    if (length > *longest)
        *longest = length;
    return lines;
}

void free_read_lines(char** readlines, int lines)
{
    if (readlines == NULL) return;
    for (int i=0; i<lines; i++)
        if (readlines[i] != NULL)
            free(readlines[i]);
    free(readlines);
}

//// free the returned char** array after use
char** read_lines(char* buffer, int* lines)
{
    int longest = 0;
    *lines = count_lines(buffer, &longest);
    char** arr = (char**)malloc(sizeof(char*)**lines);
    for (int i=0; i<*lines; i++)
        arr[i] = (char*)malloc(sizeof(char)*(longest+1));
    int i = 0;
    int j = 0;
    int n = 0;
    char c;
    while ((c = buffer[i++]) != '\0')
    {
        if (c == '\n')
        {
            arr[n][j] = '\0';
            if (j < longest+1)
                arr[n] = (char*)realloc(arr[n], j+1);
            j = 0;
            n++;
        }
        else
            arr[n][j++] = c;
    }
    return arr;
}
