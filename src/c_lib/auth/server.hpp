#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <common/files.hpp>

namespace Auth
{

// Configuration
#if PRODUCTION
#define SECRET_KEY_PATH "./data/secret_key.prod"
#else
#define SECRET_KEY_PATH "./data/secret_key.debug"
#endif
const unsigned int SECRET_KEY_SIZE = 64;
const unsigned int SECRET_KEY_REFRESH_RATE = 60;    // once per second

void server_init();
void server_teardown();
void server_update();

void received_auth_token(int client_id, const char* token);

bool verify_token(const char* _token);

void send_auth_token_valid(int client_id);
void send_auth_token_invalid(int client_id);

void send_clock_time(int client_id);

}   // Auth
