#!/usr/bin/env bash

find ../../src/ -type f -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp"  -exec sed -i 's/ *$//' '{}' ';'

