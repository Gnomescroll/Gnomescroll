#import "quad_cache.h"

#define max_cubes 1024

Vertex quad_cache[max_cubes*6*4];

struct Vertex* _get_quad_cache() {
    return &quad_cache
}
