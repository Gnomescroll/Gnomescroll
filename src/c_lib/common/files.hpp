#pragma once

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file_to_buffer(char* file_name, int* size);
off_t fsize(const char *filename);

int count_lines(char* buffer);
int count_lines(char* buffer, int* longest);

void free_read_lines(char** readlines, int lines);
char** read_lines(char* buffer);
