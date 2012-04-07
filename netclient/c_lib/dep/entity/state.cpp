#include "state.hpp"

unsigned int get_kill_reward(ObjectPolicyInterface* object, int owner, int team)
{
    ObjectState* state = object->state();
    if (
         (COINS_ANYONE  & state->coin_rule)
     || ((COINS_ENEMIES & state->coin_rule) && (object->get_team() != team))
     || ((COINS_ALLIES  & state->coin_rule) && (object->get_team() == team))
     || ((COINS_OWNER   & state->coin_rule) && (object->get_owner() == owner))
     || ((COINS_ANYONE_WHEN_UNOWNED & state->coin_rule) && (object->get_owner() == NO_AGENT))  // TODO: BAD!!
    )
        return state->reward;
    return 0;
}
