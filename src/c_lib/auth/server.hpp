#pragma once

#include <common/files.hpp>

namespace Auth
{

// Configuration
const char SECRET_KEY_PATH[] = "./data/secret_key";
const unsigned int SECRET_KEY_SIZE = 64;

void init();
void teardown();

}   // Auth
