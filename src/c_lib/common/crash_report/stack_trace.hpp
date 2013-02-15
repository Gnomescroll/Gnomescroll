#pragma once

#ifdef _WIN32
void print_trace() {}
void print_trace(int frame_start) {}
#else
# include <execinfo.h>
# include <cxxabi.h>
# define PRINT_TRACE_STACK_SIZE 32
void print_trace(int frame_start)
{
    void* array[PRINT_TRACE_STACK_SIZE];
    int size = backtrace(array, PRINT_TRACE_STACK_SIZE);
    if (frame_start >= size) return;
    char** strings = backtrace_symbols(array, size);
    if (size < PRINT_TRACE_STACK_SIZE && size > 2)
        size -= 2; // ignore main, __libc_start_main
    size_t len = 64;
    char* demangled_name = (char*)malloc(len * sizeof(char));
    int status = 0;
    printf("----------------------------\n");
    for (int i = frame_start; i < size; i++)
    {
        char* s = NULL;
        char* name_start = strchr(strings[i], '(');
        if (name_start == NULL)
            s = strings[i];
        else
        {
            s = &name_start[1];
            char* end = strchr(strings[i], '+');
            if (end != NULL)
                end[0] = '\0';
        }
        char* name = abi::__cxa_demangle(s, demangled_name, &len, &status);
        if (name != NULL)
            s = name;
        printf("%d: %s\n", i-frame_start, s);
    }
    free(demangled_name);
    free(strings);
}

void print_trace()
{
    print_trace(0);
}

# undef PRINT_TRACE_STACK_SIZE
#endif
