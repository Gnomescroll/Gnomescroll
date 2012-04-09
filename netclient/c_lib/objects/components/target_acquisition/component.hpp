#pragma once

/* TargetAcquistion component */
// TODO: move to c_lib/components/
class TargetAcquisitionComponent
{
    public:
        float target_acquisition_probability;
        Hitscan::AttackerProperties attacker_properties;
        
        void acquire_target(
            int id, Object_types type, int team, float camera_z, Vec3 position,
            float accuracy_bias, float sight_range,
            bool attack_enemies, bool attack_random
        );
    TargetAcquisitionComponent(){}
};

