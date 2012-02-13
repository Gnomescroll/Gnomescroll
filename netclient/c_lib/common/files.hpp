#pragma once

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file_to_buffer(char* file_name, int* size);
off_t fsize(const char *filename);
