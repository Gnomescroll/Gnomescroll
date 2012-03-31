#pragma once

#include <c_lib/entity/policy.hpp>

class GameObject_list: public Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>
{
    public:
        const char* name() { return "GameObject"; }
        
        void tick();
        void draw();
        void update();

        ObjectPolicyInterface* create(float x, float y, float z, float mx, float my, float mz, Object_types type);
        ObjectPolicyInterface* create(int id, float x, float y, float z, float mx, float my, float mz, Object_types type);
        void destroy(int id);

        void send_to_client(Object_types type, int client_id);
        void alter_owner(int owner, int new_owner);
        bool point_occupied_by_type(Object_types type, int x, int y, int z);
        
        GameObject_list() { print(); }
};

// TODO: restructure the list creation to adapt based on type
