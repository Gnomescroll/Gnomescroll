#pragma once

namespace serializer
{

const size_t BUF_SIZE = 0xffff-1;
extern char* _buffer;

void init_state();
void teardown_state();

}   // serializer
