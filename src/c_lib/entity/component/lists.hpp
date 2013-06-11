// X-Macro to be used by main.hpp

#define COMPONENTS \
    A(PositionPhysicsComponent, position_physics_component, POSITION, PHYSICS, MOBS_MAX) \
    A(PositionMomentumPhysicsComponent, position_momentum_physics_component, POSITION_MOMENTUM, PHYSICS, MOBS_MAX) \
    A(VerletPhysicsComponent, verlet_physics_component, VERLET, PHYSICS, MOBS_MAX) \
    A(TTLHealthComponent, ttl_health_component, TTL, HEALTH, MOBS_MAX) \
    A(HitPointsHealthComponent, hit_points_health_component, HIT_POINTS, HEALTH, MOBS_MAX) \
    A(OwnerComponent, owner_component, OWNER, OWNER, MOBS_MAX / 8) \
    A(VoxelModelComponent, voxel_model_component, VOXEL_MODEL, VOXEL_MODEL, MOBS_MAX) \
    A(MonsterSpawnerComponent, monster_spawner_component, MONSTER_SPAWNER, SPAWNER, MOBS_MAX / 8) \
    A(SpawnChildComponent, spawn_child_component, SPAWN_CHILD, SPAWN_CHILD, MOBS_MAX) \
    A(DimensionComponent, dimension_component, DIMENSION, DIMENSION, MOBS_MAX) \
    A(WeaponTargetingComponent, weapon_targeting_component, WEAPON_TARGETING, TARGETING, MOBS_MAX / 2) \
    A(MotionTargetingComponent, motion_targeting_component, MOTION_TARGETING, TARGETING, MOBS_MAX / 2) \
    A(DestinationTargetingComponent, destination_targeting_component, DESTINATION_TARGETING, TARGETING, MOBS_MAX / 2) \
    A(AgentTargetingComponent, agent_targeting_component, AGENT_TARGETING, TARGETING, MOBS_MAX / 2) \
    A(StateMachineComponent, state_machine_component, STATE_MACHINE, STATE_MACHINE, MOBS_MAX / 2) \
    A(WaitingComponent, waiting_component, WAITING, WAITING, MOBS_MAX / 2) \
    A(RateLimitComponent, rate_limit_component, RATE_LIMIT, RATE_LIMIT, MOBS_MAX) \
    C(AnimationComponent, animation_component, VOXEL_ANIMATION, ANIMATION, MOBS_MAX / 4) \
    S(AgentSpawnerComponent, agent_spawner_component, AGENT_SPAWNER, SPAWNER, MAX_SPAWNERS + 1) \
    S(ExplosionComponent, explosion_component, EXPLOSION, EXPLOSION, MOBS_MAX / 2) \
    S(ItemDropComponent, item_drop_component, ITEM_DROP, ITEM_DROP, MOBS_MAX) \
    S(HealerComponent, healer_component, HEALER, HEALING, MOBS_MAX / 4) \
    S(KnockbackComponent, knockback_component, KNOCKBACK, KNOCKBACK, MOBS_MAX)
