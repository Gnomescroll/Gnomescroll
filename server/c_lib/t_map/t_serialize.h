#pragma once

#include "t_map.h"

#include <zlib.h>
#include <assert.h>
#include <stdio.h>

extern struct vm_map map;

int _save_to_disk(char *fn);
int _load_from_disk(const char *fn);
