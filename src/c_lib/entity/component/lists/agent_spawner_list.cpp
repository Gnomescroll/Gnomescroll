#include "agent_spawner_list.hpp"

#include <entity/components/spawner/agent_spawner.hpp>

namespace Components
{

Objects::Object* AgentSpawnerComponentList::get_random_spawner()
{
    return NULL;
    
    //// TODO -- use object list filters to sort out non null
    //AgentSpawnerComponent *s;
     //objects[this->count+1];
    //int j=0;
    //for (int i=0; i<this->max; i++)
    //{   // filter down 
        //s = this->components[i];
        //if (s == NULL) continue;
        //objects[j++] = s;
    //}
    //objects[j++] = BASE_SPAWN_ID;
    //return objects[randrange(0,j-1)];
}

} // Components
