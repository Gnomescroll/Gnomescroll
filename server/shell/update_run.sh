#!/bin/bash

# update repo
hg pull; hg update

# compile & copy
./waf production
./waf
./shell/begin_server.sh
