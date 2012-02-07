#include "packets.hpp"

#ifdef DC_CLIENT
inline void ChatMessage_StoC::handle()
{
    printf("Client received: %s\n", msg);
}

inline void ChatMessage_CtoS::handle(){}
#endif

#ifdef DC_SERVER
inline void ChatMessage_CtoS::handle()
{
    printf("Server received: %s\n", msg);
}

inline void ChatMessage_StoC::handle(){}
#endif
