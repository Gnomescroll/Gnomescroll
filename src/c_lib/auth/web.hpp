#pragma once

namespace Auth
{

void login(const char* username, const char* password);

void init_curl();
void teardown_curl();

#if !GS_AUTH
void login(const char* username, const char* password) {}
void init_curl() {}
void teardown_curl() {}
#endif

}   // Auth
