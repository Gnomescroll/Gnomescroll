#include "t_serialize.h"

// 256kB buffer for compression stream
#define ZCHUNK (1024*256)

void _save_to_disk(const char* fn) {
    FILE* f = fopen(fn, "wb");
    if (f != NULL) {
        int max = vm_map_dim*vm_map_dim;
        int i,j;
        int s = sizeof(unsigned short);
        struct vm_chunk* chunk;
        for (i=0; i < max; i++) {
            chunk = map.column[i].chunk;
            for (j=0; j < vm_column_max; j++) {
                fwrite(chunk->voxel, s, 512, f);
            }
        }
        fclose(f);
    } else {
        printf("Failed to open %s for saving map\n", fn);
    }
}

void _load_from_disk(const char* fn) {
    
}
