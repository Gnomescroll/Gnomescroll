#include "packets.hpp"

#ifdef DC_CLIENT

inline void CreateSlime_StoC::handle()
{
    ClientState::slime_list->create(id,0,0,0,0,0,0);
}

inline void DestroySlime_StoC::handle()
{
    ClientState::slime_list->destroy(id);
}

inline void SlimeState_StoC::handle()
{
    Monsters::Slime* s = ClientState::slime_list->get(id);
    if (s==NULL)
    {
        //printf("SlimeState_StoC:: slime %d not found\n", id);
        return;
    }
    s->set_state(x,y,z, vx,vy,vz, theta, phi);
}

#endif

#ifdef DC_SERVER

inline void CreateSlime_StoC::handle(){}
inline void DestroySlime_StoC::handle(){}
inline void SlimeState_StoC::handle(){}

#endif
