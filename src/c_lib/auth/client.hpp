#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <auth/constants.hpp>

namespace Auth
{

extern char* auth_token;
extern int auth_token_user_id;
extern time_t auth_token_timestamp;
extern char* auth_token_hash;
extern char* auth_token_username;

extern bool authorized;
extern bool has_authorized_once;
extern bool needs_login;
extern bool refreshing_token;
extern bool token_failure;

extern bool token_available;

bool send_auth_token(); // returns true if token was sent
bool send_auth_token(const char* token); // returns true if token was sent

AuthError update_token(const char* token);

void token_was_accepted();
void token_was_denied();

void client_init();
void client_teardown();

void check_expiring_token();

}   // Auth
