#include "files.hpp"

off_t fsize(const char *filename)
{
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

char* read_file_to_buffer(char* file_name, int* size)
{
    int expected_size = fsize(file_name);
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
    //free(source); /* Don't forget to call free() later! */
    return source;
}
