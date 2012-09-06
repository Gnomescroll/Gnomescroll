#include "packets.hpp"

#if DC_SERVER
#include <auth/server.hpp>
#endif

namespace Auth
{

#if DC_CLIENT
inline void auth_token_CtoS::handle() {}
#endif

#if DC_SERVER
inline void auth_token_CtoS::handle()
{
    received_auth_token(client_id, this->token);
}
#endif

}   // Auth
