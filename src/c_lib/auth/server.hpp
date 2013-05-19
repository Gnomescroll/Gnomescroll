#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <common/files.hpp>

namespace Auth
{

// Configuration
#if PRODUCTION
# define SECRET_KEY_PATH "./keys/secret_key.prod"
#else
# define SECRET_KEY_PATH "./keys/secret_key.debug"
#endif

#define SECRET_KEY_PATH_TMP_EXT ".new"

const unsigned int SECRET_KEY_SIZE = 64;
const unsigned int SECRET_KEY_REFRESH_RATE = 60;    // once per second

void server_init();
void server_teardown();
void server_update();

void received_auth_token(ClientID client_id, const char* token);

bool verify_token(const char* _token);

void send_auth_token_valid(ClientID client_id);
void send_auth_token_invalid(ClientID client_id);

void send_clock_time(ClientID client_id);

}   // Auth
