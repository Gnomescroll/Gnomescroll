#pragma once

#include <stdlib.h>

// strcpy but dont add \0 to the end of a
//void strcpy_no_null(char* dest, char* src);

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

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
/* source: http://benpfaff.org/writings/clc/shuffle.html */
void shuffle_int_array(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
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
