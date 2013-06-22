// X-Macro to be used by main.hpp

// list of (TYPE, INTERFACE) names
// A means ALL, C means CLIENT, S means SERVER
#define COMPONENTS \
    A(Position, Physics) \
    A(PositionMomentum, Physics) \
    A(Verlet, Physics) \
    A(TTL, Health) \
    A(HitPoints, Health) \
    A(Owner, Owner) \
    A(VoxelModel, VoxelModel) \
    A(MonsterSpawner, Spawner) \
    A(SpawnChild, SpawnChild) \
    A(Dimension, Dimension) \
    A(WeaponTargeting, Targeting) \
    A(MotionTargeting, Targeting) \
    A(DestinationTargeting, Targeting) \
    A(AgentTargeting, Targeting) \
    A(StateMachine, StateMachine) \
    A(Waiting, Waiting) \
    A(RateLimit, RateLimit) \
    A(SpriteMob, SpriteMob) \
    C(Animation, Animation) \
    S(AgentSpawner, Spawner) \
    S(Explosion, Explosion) \
    S(ItemDrop, ItemDrop) \
    S(Healer, Healer) \
    S(Knockback, Knockback)


// list of (TYPE, CALL_RATE=1)
// these will be scheduled by the main tick
#define CALL_LIST \
    S(RateLimit) \
    S(MotionTargeting) \
    S(AgentTargeting) \
    S(Knockback) \
    S(Healer, 6) \
    C(Position) \
    C(PositionMomentum)
