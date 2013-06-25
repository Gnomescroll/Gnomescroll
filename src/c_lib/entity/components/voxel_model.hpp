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
        bool frozen;
        bool init_hitscan;
        bool init_draw;
        bool should_hitscan;
        bool should_draw;

    void load_settings_from(const Component* component)
    {
        BEGIN_COPY(VoxelModelComponent);
        COPY(vox_dat);
        COPY(init_hitscan);
        COPY(init_draw);
        COPY(should_hitscan);
        COPY(should_draw);
    }

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
        if (this->vox == NULL) return vec3_init(0);
        return this->vox->get_center();
    }

    struct Vec3 get_center(int part)
    {
        if (this->vox == NULL) return vec3_init(0);
        Voxels::VoxelVolume* vv = vox->get_part(part);
        if (vv == NULL) return vec3_init(0);
        return vv->get_center();
    }

    void ready(const Vec3& position, float theta, float phi)
    {
        IF_ASSERT(this->vox != NULL) return;
        IF_ASSERT(this->vox_dat == NULL) return;
        this->vox = new Voxels::VoxelModel(this->vox_dat, this->entity->id, this->entity->type);
        this->set_properties();
        vox->update(position, theta, phi);
        vox->set_hitscan(this->init_hitscan);
        vox->set_draw(this->init_draw);
    }

    void freeze()
    {
        if (this->vox != NULL)
            this->vox->freeze();
    }

    virtual void on_ready()
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
        IF_ASSERT(physics == NULL) return;
        Vec3 angles = physics->get_angles();
        this->ready(physics->get_position(), angles.x, angles.y);
        if (this->frozen)
            this->freeze();
    }

    virtual void on_update()
    {
        auto physics = GET_COMPONENT_INTERFACE(Physics, this->entity);
        IF_ASSERT(physics == NULL) return;
        Vec3 angles = physics->get_angles();
        if (this->frozen)
            this->force_update(physics->get_position(), angles.x, angles.y, physics->get_changed());
        else
            this->update(physics->get_position(), angles.x, angles.y, physics->get_changed());
        physics->set_changed(false);    // the thing that uses it should unset changed state
                                        // because too much hassle guaranteeing on_update() ordering,
                                        // or we'd put it on the physics component
                                        // Note that this implies the changed state is
                                        // only usable by one component
    }

    void update(const Vec3& position, float theta, float phi, bool state_changed);
    void force_update(const Vec3& position, float theta, float phi, bool state_changed);

    virtual ~VoxelModelComponent()
    {
        delete this->vox;
    }

    VoxelModelComponent() :
        Component(COMPONENT_VoxelModel, COMPONENT_INTERFACE_VoxelModel),
        vox(NULL), vox_dat(NULL), frozen(false), init_hitscan(false),
        init_draw(false), should_hitscan(true), should_draw(true)
    {}
};

} // Entities
