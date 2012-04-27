#include "CtoS.hpp"

#if DC_SERVER
#include <state/server_state.hpp>
#include <item/_interface.hpp>
#endif

namespace Item
{


#if DC_SERVER

inline void move_item_to_hand_CtoS::handle() 
{

}

inline void drop_item_from_hand_CtoS::handle() 
{

}

#endif



#if DC_CLIENT

inline void move_item_to_hand_CtoS::handle() {} 
inline void drop_item_from_hand_CtoS::handle() {}

#endif


}
