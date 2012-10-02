#pragma once

#include <hiredis/hiredis.h>
#include <hiredis/async.h>


namespace serializer
{
namespace redis
{

extern bool connected; 

void init();
void teardown();
void update();

}   // redis
}   // serializer
