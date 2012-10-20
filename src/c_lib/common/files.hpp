#pragma once

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/macros.hpp>

int GS_MKDIR(const char* dir, int permissions);

char* read_file_to_buffer(const char* file_name, size_t* size);
char* read_binary_file_to_buffer(const char* filename, size_t* size);

off_t fsize(const char *filename);

int count_lines(char* buffer);
int count_lines(char* buffer, int* longest);

void free_read_lines(char** readlines, int lines);
char** read_lines(char* buffer);

void create_path(const char* path);
void create_path_to_file(const char* fn);

bool file_exists(const char* filename);
