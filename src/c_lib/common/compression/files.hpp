#pragma once

int compress_buffer_to_file(const char* filename, const char* inbuf, const size_t buffer_size);
char* decompress_file_to_buffer(const char* filename, size_t& len);
