#pramga once


#include <c_lib/common/enum_types.hpp>
#include <c_lib/monsters/constants.hpp>
#include <c_lib/objects/components/target_acquisition/component.hpp>
#include <c_lib/objects/components/voxel/component.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/net/packets.hpp>

#include <c_lib/monsters/slime.hpp>

namespace Monsters {

extern VoxDat box_vox_dat;


class Box: public VoxelComponent, public MonsterInterface
{
    public:

    void tick(){}

    void die()
    {
        #if DC_CLIENT
        //dieAnimation();   // todo
        if (this->voxel_properties.vox == NULL)
        {
            Vec3 position = this->voxel_properties.vox->get_part(BOX_PART_BODY)->get_center();
            Animations::slime_melt(position.x, position.y, position.z); // TODO
        }
        #endif

        #if DC_SERVER
        //boxDropItem(this->get_position());
        this->broadcastDeath();
        #endif
    }

    void born()
    {
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornVox(this->voxel_properties.vox_dat, state->id, state->type);
        bornSetVox(
            this->voxel_properties.vox,
            this->voxel_properties.init_hitscan,
            this->voxel_properties.init_draw
        );
        bornUpdateVox(this->voxel_properties.vox, this->get_position(),
            this->spatial_properties.angles.x, this->spatial_properties.angles.y); 
    }

    void update()
    {
        updateVox(
            this->voxel_properties.vox, this->get_position(),
            this->spatial_properties.angles, this->spatial_properties.changed
        );
        this->spatial_properties.set_changed(false);
    }

    
    void draw(){/*Empty*/}

    explicit Box(int id)
    {
        this->_state.id = id;

        this->_state.coin_rule = COINS_ANYONE;

        this->_state.motion_proximity_radius = BOX_MOTION_PROXIMITY_RADIUS;

        // todo -- firing rate / properties

        this->voxel_properties.init_hitscan = true;
        this->voxel_properties.init_draw = true;
        this->voxel_properties.vox_dat = &box_vox_dat;

        this->spatial_properties.camera_height = BOX_CAMERA_HEIGHT;
        this->spatial_properties.height = BOX_HEIGHT;

        this->health_properties.health = BOX_HEALTH;

        // TODO -- make speed a base property
        // momentum should not be used this way (can be overwriiten, is only init etc)
        this->set_momentum(BOX_SPEED, BOX_SPEED, BOX_SPEED);
    }
}

} // Monsters
