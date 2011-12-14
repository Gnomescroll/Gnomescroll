#pragma once

#define N_FLAGS 2 

#include <c_lib/template/object_list.hpp>

class Flag_list: public Object_list<Flag,N_FLAGS>
{
    private:
        const char* name() { return "Flag"; }
    public:
        void draw();
};
