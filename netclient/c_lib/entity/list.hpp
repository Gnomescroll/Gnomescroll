#pragma once

#include <c_lib/entity/policy.hpp>

class GameObject_list: public Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>
{
    public:
        const char* name() { return "GameObject"; }
        
        void tick();
        void draw();
        ObjectPolicyInterface* create(float x, float y, float z, float mx, float my, float mz, Object_types type);
        void destroy(int id);

        void alter_owner(int owner, int new_owner);
        
        GameObject_list() { print(); }
};

// TODO: restructure the list creation to adapt based on type
