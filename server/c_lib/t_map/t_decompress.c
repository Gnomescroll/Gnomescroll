#include "t_decompress.h"



int map_load_from_disk(const char* fn) {

    // open file for reading

    FILE* f = fopen(fn, "rb");
    if (f != NULL) {

        

        fclose(f);
        return 0;
    }

    printf("Failed to open map file %s\n", fn);
    return 1;
}
