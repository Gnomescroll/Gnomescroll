#include "state.hpp"

int ObjectState::get_team()
{
    return this->team;
}

void ObjectState::set_team(int team)
{
    this->team = team;
}

int ObjectState::get_owner()
{
    return this->owner;
}

void ObjectState::set_owner(int owner)
{
    switch_agent_ownership(this->type, this->owner, owner);
    this->owner = owner;
}

int ObjectState::get_coins_for_kill(int owner, int team)
{   // TODO: allow objects to configure this behaviour
    if ((this->team != team && this->owner != NO_AGENT) || owner == this->owner) // enemy team, or owner, can destroy/reclaim
        return get_object_cost(this->type);
    return 0;
}

int ObjectState::take_damage(int dmg)
{
    if (this->health <= 0) return 0;
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;
    if (this->health <= 0)
        this->ttl = this->ttl_max;
    return this->health;
}
