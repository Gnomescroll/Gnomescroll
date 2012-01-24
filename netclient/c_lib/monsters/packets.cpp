#include "packets.hpp"

#ifdef DC_CLIENT

inline void CreateSlime_StoC::handle()
{
    Monsters::Slime* s = ClientState::slime_list.create(id);
    if (s->vox != NULL) {
        s->vox->set_draw(true);
        s->vox->update(&Monsters::slime_vox_dat, s->x, s->y, s->z, s->theta, s->phi);
    }
}

inline void DestroySlime_StoC::handle()
{
    ClientState::slime_list.destroy(id);
}

inline void SlimeState_StoC::handle()
{
    Monsters::Slime* s = ClientState::slime_list.get(id);
    if (s==NULL) return;
    s->set_state(x,y,z, vx,vy,vz, theta, phi);
}

#endif

#ifdef DC_SERVER

inline void CreateSlime_StoC::handle(){}
inline void DestroySlime_StoC::handle(){}
inline void SlimeState_StoC::handle(){}

#endif
