#include "packets.hpp"

#if DC_SERVER
#include <auth/server.hpp>
#endif
#if DC_CLIENT
#include <auth/client.hpp>
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

inline void clock_time_StoC::handle()
{
    set_time_offset((time_t)this->server_time);
}

inline void auth_token_CtoS::handle() {}
#endif

#if DC_SERVER
inline void auth_token_valid_StoC::handle() {}
inline void auth_token_invalid_StoC::handle() {}
inline void clock_time_StoC::handle() {}

inline void auth_token_CtoS::handle()
{
    received_auth_token(client_id, this->token);
}
#endif

}   // Auth
