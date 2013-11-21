/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#ifndef __APPLE__
dont_include_this_file_in_non_OSX_builds
#endif

#include <common/crash_report/stack_trace.hpp>
#include <common/macros.hpp>
#include <common/gs_assert.hpp>

// copied from
// http://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#include <unistd.h>
#endif

// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
#ifdef __APPLE__
void set_working_directory_to_Resources()
{
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    bool success = CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX);
    GS_ASSERT(success);
    CFRelease(resourcesURL);

    chdir(path);
    printf("Working directory set to: %s\n", path);
}
#endif
// ----------------------------------------------------------------------------
