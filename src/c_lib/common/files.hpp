#pragma once

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file_to_buffer(const char* file_name, int* size);
char* fast_read_file_to_buffer(const char* file_name, int* size);

off_t fsize(const char *filename);

int count_lines(char* buffer);
int count_lines(char* buffer, int* longest);

void free_read_lines(char** readlines, int lines);
char** read_lines(char* buffer);

void create_directory(const char* directory);
void create_path_to_file(const char* fn);
