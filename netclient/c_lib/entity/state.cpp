#include "state.hpp"

int ObjectState::get_kill_reward(int owner, int team)
{
    if (
         (COINS_ANYONE  & this->coin_rule)
     || ((COINS_ENEMIES & this->coin_rule) && (this->team != team))
     || ((COINS_ALLIES  & this->coin_rule) && (this->team == team))
     || ((COINS_OWNER   & this->coin_rule) && (this->owner == owner))
     || ((COINS_ANYONE_WHEN_UNOWNED & this->coin_rule) && (this->owner == NO_AGENT))
    )
        return this->reward;
    return 0;
}

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

int ObjectState::take_damage(int dmg)
{
    if (this->health <= 0) return 0;
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;
    if (this->health <= 0)
        this->ttl = this->ttl_max;
    return this->health;
}
