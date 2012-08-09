#include "files.hpp"

#include <sys/stat.h>

#include <common/macros.hpp>

#ifdef __WIN32__
#ifdef __GNUC__
    #include <io.h>
#endif
#endif

int GS_MKDIR(const char* dir, int permissions)
{
#ifdef _WIN32
    #ifdef __MSVC__
        return _mkdir(dir);
    #else
        return mkdir(dir);
    #endif
#else
    return mkdir(dir, S_IRWXU);
#endif
}

off_t fsize(const char *filename)
{
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

// free the returned char* buffer after use
char* read_file_to_buffer(const char* file_name, int* size)
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

//possible unsafe, free buffer after use
char* fast_read_file_to_buffer(const char* file_name, int* size)
{
    long bufsize = (int)fsize(file_name);
    char *source = NULL;
    FILE *fp = fopen(file_name, "r");
    if (fp != NULL)
    {
        *size = (int)bufsize;
        /* Allocate our buffer to that size. */
        source = (char*) calloc(bufsize+2, sizeof(char));
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

        if(fp == NULL) printf("WTF\n");
        fclose(fp);
    }
    else
    {
        printf("read_file_to_buffer: error, could not open %s \n", file_name);
        GS_ABORT();
    }
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
    int lines = 0;
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

#ifdef _WIN32
static const char SEPARATOR = '\\';
#else
static const char SEPARATOR = '/';
#endif

void create_path(const char* fn)
{
    // walk down the path, create folders as needed
    int len = strlen(fn);
    char* path = (char*)malloc((len+1) * sizeof(char));
    strcpy(path, fn);

    struct stat file_stat;
    
    char c;
    int i=0;
    while ((c = path[i++]) != '\0')
    {
        if (c != SEPARATOR) continue;
        path[i-1] = '\0';
        if (stat(path, &file_stat) != 0)
        { 
            GS_MKDIR(path, 0777);
        }
        path[i-1] = SEPARATOR;
    }

    if (stat(path, &file_stat) != 0)
        GS_MKDIR(path, 0777);

    free(path);
}

void create_path_to_file(const char* fn)
{
    int i = ((int)strlen(fn)) - 1;
    for (; i>=0; i--)
        if (fn[i] == SEPARATOR)
            break;
    GS_ASSERT(i >= 0);
    if (i < 0) return;

    char* path = (char*)malloc((i+1) * sizeof(char));
    int j=0;
    for (; j<i; j++) path[j] = fn[j];
    path[j] = '\0';
    create_path(path);
    free(path);
}

bool file_exists(const char* filename)
{
    struct stat file_stat;
    return (stat(filename, &file_stat) == 0);
}
