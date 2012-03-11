#pragma once

#include <c_lib/voxel/voxel_model.hpp>
#include <agent/agent_status.hpp>

extern VoxDat agent_vox_dat;
extern VoxDat agent_vox_dat_crouched;
extern VoxDat agent_vox_dat_dead;

enum AGENT_BODY_PARTS
{
    AGENT_PART_TORSO,
    AGENT_PART_HEAD,
    AGENT_PART_LARM,
    AGENT_PART_RARM,
    AGENT_PART_LLEG,
    AGENT_PART_RLEG
};

const int AGENT_PART_HEAD_DAMAGE  = AGENT_HEALTH;
const int AGENT_PART_TORSO_DAMAGE = AGENT_HEALTH / 2;
const int AGENT_PART_LARM_DAMAGE  = AGENT_HEALTH / 4;
const int AGENT_PART_RARM_DAMAGE  = AGENT_HEALTH / 4;
const int AGENT_PART_LLEG_DAMAGE  = AGENT_HEALTH / 4;
const int AGENT_PART_RLEG_DAMAGE  = AGENT_HEALTH / 4;

const int SPARE_AGENT_MODEL_MAX = 6;
const int SPARE_AGENT_MODEL_DRAW_TIMEOUT = 30 * 3; // 3 seconds

class Agent_voxel_model: public Voxel_model
{
    private:
        int ttl;
        bool rendered;
    public:
        void tick()
        {
            if (!this->rendered) return;
            if (this->ttl++ > SPARE_AGENT_MODEL_DRAW_TIMEOUT)
            {
                this->rendered = false;
                this->set_draw(false);
            }
        }
        void reset()
        {
            this->ttl = 0;
            this->rendered = false;
        }
        Agent_voxel_model(VoxDat* vox_dat, int id, int type)
        :
        Voxel_model(vox_dat, id, type)
        {
            this->reset();
        }

};

#include <c_lib/template/object_list.hpp>

class SpareAgentModelList: public Object_list<Agent_voxel_model, SPARE_AGENT_MODEL_MAX>
{
    private:
        const char* name() { return "SpareAgentModel"; }
    public:

        void tick()
        {
            for (int i=0; i<this->n_max; i++)
                if (this->a[i] != NULL)
                    this->a[i]->tick();
        }

        SpareAgentModelList() { print(); }
};
