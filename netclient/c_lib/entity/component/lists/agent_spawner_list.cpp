#include "agent_spawner_list.hpp"

#include <c_lib/components/spawner/agent_spawner.hpp>
#include <c_lib/components/team.hpp>

namespace Components
{

bool AgentSpawnerComponentList::team_spawner_available(int team)
{
    AgentSpawnerComponent *s;
    int n = 0;
    for (int i=0; i<this->max; i++)
    {
        s = this->components[i];
        if (s == NULL) continue;
        TeamComponent* team = (TeamComponent*)s->object->get_component_interface(COMPONENT_INTERFACE_TEAM);
        if (team != NULL && team->get_team() == team) n++;
    }
    return (n < this->max_per_team);
}

int AgentSpawnerComponentList::get_random_spawner(int team)
{
    AgentSpawnerComponent *s;
    int objects[this->max_per_team+1];
    int j=0;
    for (int i=0; i<this->max; i++)
    {   // filter down to team's objects
        s = this->components[i];
        if (s == NULL) continue;
        IndexedTeamComponent* team = (IndexedTeamComponent*)s->object->get_component(COMPONENT_INDEXED_TEAM);
        if (team != NULL && team->get_team() == team)
            objects[j++] = team->get_team_index();
    }
    objects[j++] = BASE_SPAWN_ID;
    return objects[randrange(0,j-1)];
}

// when objects player says "spawner 8" he may be on the other team
// we need to find the 8th spawner for his team
int AgentSpawnerComponentList::get_numbered_team_spawner(int team, int id)
{
    AgentSpawnerComponent *s;
    for (int i=0; i<this->max; i++)
    {
        s = this->components[i];
        if (s == NULL) continue;
        IndexedTeamComponent* team = (IndexedTeamComponent*)s->object->get_component(COMPONENT_INDEXED_TEAM);
        if (team->get_team() != team) continue;
        if ((int)team->get_team_index() == id)
            return s->object->id;
    }
    return BASE_SPAWN_ID;
}

Object* AgentSpawnerComponentList::get_by_team_index(int team, int team_index)
{
    AgentSpawnerComponentList *s;
    for (int i=0; i<this->max; i++)
    {
        s = this->components[i];
        if (s == NULL) continue;
        IndexedTeamComponent* team = (IndexedTeamComponent*)s->object->get_component(COMPONENT_INDEXED_TEAM);
        if (team->get_team() != team) continue;
        if ((int)team->get_team_index() == team_index)
            return s->object;
    }
    return NULL;
}

bool AgentSpawnerComponentList::spawner_exists(int team, int team_index)
{
    if (this->get_by_team_index(team, team_index) != NULL)
        return true;
    return false;
}

void AgentSpawnerComponentList::assign_team_index(Object* spawner)
{   // pick an index for the spawner that is available, these are separate from
    // id because each team's set of objects has its own indexing
    // and objects may be destroyed; we dont want to renumber every time
    // get smallest available team index
    if (!this->max) return;
    int* taken = (int*)calloc(this->max, sizeof(int));
    int team_index = TEAM_INDEX_NONE;
    AgentSpawnerComponent* s;

    IndexedTeamComponent* team = (IndexedTeamComponent*)spawner->get_component(COMPONENT_INDEXED_TEAM);
    unsigned int spawner_team_index = team->get_team_index();
    int spawner_team = team->get_team();

    for (int i=0; i<this->max; i++)
    {
        s = this->components[i];
        if (s == NULL) continue;
        IndexedTeamComponent* team = (IndexedTeamComponent*)s->object->get_component(COMPONENT_INDEXED_TEAM);
        if (team->get_team() != spawner_team) continue;
        taken[spawner_team_index - 1] = 1;
    }
    for (int i=0; i<this->max; i++)
        if (!taken[i])
        {
            team_index = i+1;
            break;
        }
    team->set_team_index(team_index);
    free(taken);
}


} // Components
