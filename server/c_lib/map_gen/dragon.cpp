#include "dragon.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/map_gen/features.h>

namespace Dragon {

const int x = 128;
const int y = 128;

void generate() {
    _floor(x,y, 0, 1, 11);
}

//cython
void generate_dragon() {
    generate();
}
}
