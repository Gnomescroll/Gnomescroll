#pragma once

#include <common/template/object_list.hpp>
#include <t_gen/explosives.hpp>

namespace Particle
{

const int PLASMAGEN_SPUR_MAX = 1024;
const float PLASMAGEN_SPUR_DAMP = 1.0f;
const int PLASMAGEN_SPUR_TEXTURE_ID = 22;
const float PLASMAGEN_SPUR_TEXTURE_SCALE = 0.25f;
const float PLASMAGEN_MOMENTUM = 10.0f;

class PlasmagenSpur: public ParticleMotion, public BillboardSprite
{
    public:
        float travelled;

    inline void reset();
    void tick();
    void die();

    #if DC_SERVER
    void broadcast_death();
    void broadcast_birth();
    #endif

    explicit PlasmagenSpur(int id);
};

class plasmagen_explode_StoC: public FixedSizeReliableNetPacketToClient<plasmagen_explode_StoC>
{
    public:
        struct Vec3 position;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_vec3(&position, buff, buff_n, pack);
    }
    inline void handle();
};

class PlasmagenSpurList: public ObjectList<PlasmagenSpur>
{
    private:
        const char* name() { return "PlasmagenSpur"; }
    public:
        #if DC_CLIENT
        void draw();
        #endif
        void tick();

    PlasmagenSpurList(unsigned int capacity) :
        ObjectList<PlasmagenSpur>(capacity)
    {}
};

}   // Particle
