#include "packets.hpp"

#if DC_SERVER
#include <auth/server.hpp>
#endif

namespace Auth
{

#if DC_CLIENT
inline void auth_token_valid_StoC::handle()
{
    token_was_accepted();
}

inline void auth_token_invalid_StoC::handle()
{
    token_was_denied();
}

inline void auth_token_CtoS::handle() {}
#endif

#if DC_SERVER
inline void auth_token_valid_StoC::handle() {}
inline void auth_token_invalid_StoC::handle() {}

inline void auth_token_CtoS::handle()
{
    received_auth_token(client_id, this->token);
}
#endif

}   // Auth
