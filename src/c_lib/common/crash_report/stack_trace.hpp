#pragma once

#ifdef _WIN32
void print_trace() {}
void print_trace(int frame_start) {}
#else
# include <execinfo.h>
# define PRINT_TRACE_STACK_SIZE 15
void print_trace(int frame_start)
{
    void *array[PRINT_TRACE_STACK_SIZE];
    int size;
    char **strings;
    
    size = backtrace(array, PRINT_TRACE_STACK_SIZE);
    if (frame_start >= size) return;

    strings = backtrace_symbols(array, size);

    for (int i = frame_start; i < size; i++)
        puts(strings[i]);

    free(strings);
}

void print_trace()
{
    print_trace(0);
}

# undef PRINT_TRACE_STACK_SIZE
#endif
