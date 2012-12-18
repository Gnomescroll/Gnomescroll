#pragma once

#include <voxel/voxel_model.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class VoxelModelComponent: public Component
{
    private:
            // Initializes vox model properties. almost always desired
        void set_properties()
        {
            if (this->vox == NULL) return;
            this->vox->set_hitscan(this->init_hitscan);
            this->vox->register_hitscan();
            #if DC_CLIENT
            this->vox->set_draw(this->init_draw);
            #endif
        }

    public:
        Voxels::VoxelModel* vox;
        Voxels::VoxDat* vox_dat;
        bool init_hitscan;
        bool init_draw;
        bool should_hitscan;
        bool should_draw;

        float get_radius()
        {
            return this->get_radius(0);
        }
        
        float get_radius(int part)
        {
            if (this->vox == NULL) return 1.0f;
            Voxels::VoxelVolume* vv = vox->get_part(part);
            if (vv == NULL) return 1.0f;
            return vv->radius;
        }

        struct Vec3 get_center()
        {
            if (this->vox == NULL) return vec3_init(0,0,0);
            return this->vox->get_center();
        }
        
        struct Vec3 get_center(int part)
        {
            if (this->vox == NULL) return vec3_init(0,0,0);
            Voxels::VoxelVolume* vv = vox->get_part(part);
            if (vv == NULL) return vec3_init(0,0,0);
            return vv->get_center();
        }

        void ready(Vec3 position, float theta, float phi)
        {
            GS_ASSERT(this->vox == NULL);
            if (this->vox != NULL) return;
            GS_ASSERT(this->vox_dat != NULL);
            if (this->vox_dat == NULL) return;
            this->vox = new Voxels::VoxelModel(this->vox_dat, this->object->id, this->object->type);
            this->set_properties();
            vox->update(position.x, position.y, position.z, theta, phi);
            vox->set_hitscan(this->init_hitscan);
            vox->set_draw(this->init_draw);
        }

        void freeze()
        {
            if (this->vox == NULL) return;
            this->vox->freeze();
        }

        void update(Vec3 position, float theta, float phi, bool state_changed);
        void force_update(Vec3 position, float theta, float phi, bool state_changed);

    ~VoxelModelComponent()
    {
        if (this->vox != NULL) delete this->vox;
    }

    VoxelModelComponent()
    : Component(COMPONENT_VOXEL_MODEL, COMPONENT_INTERFACE_VOXEL_MODEL),
    vox(NULL), vox_dat(NULL),
    init_hitscan(false), init_draw(false), should_hitscan(true), should_draw(true)
    {}
};

} // Entities
