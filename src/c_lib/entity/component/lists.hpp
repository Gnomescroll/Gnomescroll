// X-Macro to be used by main.hpp

// TODO -- compute component list sizes from needs of mob list sizes

#define COMPONENTS \
    A(PositionComponent, position_physics_component, Position, Physics, MOBS_MAX) \
    A(PositionMomentumComponent, position_momentum_physics_component, PositionMomentum, Physics, MOBS_MAX) \
    A(VerletComponent, verlet_physics_component, Verlet, Physics, MOBS_MAX) \
    A(TTLComponent, ttl_component, TTL, Health, MOBS_MAX) \
    A(HitPointsComponent, hit_points_component, HitPoints, Health, MOBS_MAX) \
    A(OwnerComponent, owner_component, Owner, Owner, MOBS_MAX / 8) \
    A(VoxelModelComponent, voxel_model_component, VoxelModel, VoxelModel, MOBS_MAX) \
    A(MonsterSpawnerComponent, monster_spawner_component, MonsterSpawner, Spawner, MOBS_MAX / 8) \
    A(SpawnChildComponent, spawn_child_component, SpawnChild, SpawnChild, MOBS_MAX) \
    A(DimensionComponent, dimension_component, Dimension, Dimension, MOBS_MAX) \
    A(WeaponTargetingComponent, weapon_targeting_component, WeaponTargeting, Targeting, MOBS_MAX / 2) \
    A(MotionTargetingComponent, motion_targeting_component, MotionTargeting, Targeting, MOBS_MAX / 2) \
    A(DestinationTargetingComponent, destination_targeting_component, DestinationTargeting, Targeting, MOBS_MAX / 2) \
    A(AgentTargetingComponent, agent_targeting_component, AgentTargeting, Targeting, MOBS_MAX / 2) \
    A(StateMachineComponent, state_machine_component, StateMachine, StateMachine, MOBS_MAX / 2) \
    A(WaitingComponent, waiting_component, Waiting, Waiting, MOBS_MAX / 2) \
    A(RateLimitComponent, rate_limit_component, RateLimit, RateLimit, MOBS_MAX) \
    A(SpriteMobComponent, sprite_mob_component, SpriteMob, SpriteMob, MOBS_MAX) \
    C(AnimationComponent, animation_component, Animation, Animation, MOBS_MAX / 4) \
    S(AgentSpawnerComponent, agent_spawner_component, AgentSpawner, Spawner, MAX_SPAWNERS + 1) \
    S(ExplosionComponent, explosion_component, Explosion, Explosion, MOBS_MAX / 2) \
    S(ItemDropComponent, item_drop_component, ItemDrop, ItemDrop, MOBS_MAX) \
    S(HealerComponent, healer_component, Healer, Healer, MOBS_MAX / 4) \
    S(KnockbackComponent, knockback_component, Knockback, Knockback, MOBS_MAX)
