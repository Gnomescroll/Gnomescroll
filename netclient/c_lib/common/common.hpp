#pragma once

void address_from_string(char* ip, int address[4]);

// strcpy but dont add \0 to the end of a
//void strcpy_no_null(char* dest, char* src);

unsigned int sanitize_player_name(char* name);

struct Color {
    unsigned char r,g,b;
};

bool in_array_int(int* arr, int arr_max, int val)
{
    for (int i=0; i<arr_max; i++)
        if (arr[i] == val) return true;
    return false;
}
