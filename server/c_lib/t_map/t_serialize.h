#pragma once

#include "t_map.h"

#include <zlib.h>
#include <assert.h>
#include <stdio.h>

extern struct vm_map map;

int map_save_to_disk(char *fn);
int map_load_from_disk(const char *fn);
