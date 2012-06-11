#pragma once 

#ifndef __WIN32__
#include <execinfo.h>

#define PRINT_TRACE_STACK_SIZE 15
void print_trace()
{
    void *array[PRINT_TRACE_STACK_SIZE];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, PRINT_TRACE_STACK_SIZE);
    strings = backtrace_symbols(array, size);

    //printf ("Obtained %zd stack frames.\n", size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);

    free(strings);
}
#undef PRINT_TRACE_STACK_SIZE
#else
void print_trace() {}
#endif
