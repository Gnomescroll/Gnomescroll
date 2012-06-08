#pragma once

#include <stdlib.h>
#include <stdint.h>

unsigned int sanitize_player_name(char* name);

struct Color {
    unsigned char r,g,b;
};

struct Color4 {
    unsigned char r,g,b,a;
};

bool in_array_int(int* arr, int arr_max, int val)
{
    for (int i=0; i<arr_max; i++)
        if (arr[i] == val) return true;
    return false;
}

int count_digits(int n)
{
    int digits = 0;
    do {
        digits++;
        n /= 10;
    } while (n > 0);
    return digits;
}

void address_from_string(char* ip, int address[4]);
void address_from_uint32(uint32_t ip, uint8_t address[4]);
