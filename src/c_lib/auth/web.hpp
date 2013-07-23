#pragma once

namespace Auth
{

bool login(const char* username, const char* password);
bool create_account(const char* username, const char* email, const char* password);
bool check_version();

void init_curl();
void teardown_curl();

#if !GS_AUTH
bool check_version() {}
bool login(const char* username, const char* password) {}
bool create_account(const char* username, const char* email, const char* password) {}
void init_curl() {}
void teardown_curl() {}
#endif

}   // Auth
