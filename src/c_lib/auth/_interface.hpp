#pragma once

namespace Auth
{

void init();
void teardown();

void init_packets();

bool parse_auth_token(const char* token, int* user_id, time_t* expiration_time, char** hash);

bool auth_token_expired(const time_t timestamp, const time_t expiration_window);

}   // Auth
