#pragma once

#if GS_SERIALIZER

namespace serializer
{

void init();
void teardown();
void update();

// defined in map.cpp
void load_map(const char* filename);
void save_map();
void wait_for_threads();

}   // serializer

#else

// stubs
namespace serializer
{

bool should_save_map = false;

void init(){}
void teardown(){}

void check_save_state(){}

void load_map(const char* filename){}
void save_map(){}
void wait_for_threads(){}

}   // serializer

#endif
