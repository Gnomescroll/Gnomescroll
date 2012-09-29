#pragma once

namespace serializer
{

void init();
void teardown();

void check_save_state();

void load_map(const char* filename);
void save_map();
void wait_for_threads();

}   // serializer
