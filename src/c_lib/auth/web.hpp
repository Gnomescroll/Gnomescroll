#pragma once

namespace Auth
{

void download_homepage();

#if !GS_AUTH
void download_homepage() {}
#endif

}   // Auth
