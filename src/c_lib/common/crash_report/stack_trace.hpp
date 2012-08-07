#pragma once 

#ifdef __MSVC__
	void print_trace() {}

#else
	#ifndef __WIN32__
		#include <execinfo.h>

		#define PRINT_TRACE_STACK_SIZE 15
		void print_trace()
		{
			void *array[PRINT_TRACE_STACK_SIZE];
			int size;
			char **strings;

			size = backtrace(array, PRINT_TRACE_STACK_SIZE);
			strings = backtrace_symbols(array, size);

			//printf ("Obtained %zd stack frames.\n", size);
			for (int i = 0; i < size; i++)
				puts(strings[i]);
				//printf("%s\n", strings[i]);

			free(strings);
		}
		#undef PRINT_TRACE_STACK_SIZE
	#else
		void print_trace() {}
	#endif

#endif
