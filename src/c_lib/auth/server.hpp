#pragma once

#include <common/files.hpp>

namespace Auth
{

// Configuration
const char SECRET_KEY_PATH[] = "./data/secret_key";
const unsigned int SECRET_KEY_SIZE = 64;

const time_t AUTH_TOKEN_LIFETIME = 60 * 60; // 1 hour, in seconds 

void init();
void teardown();

}   // Auth
