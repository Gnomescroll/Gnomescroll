#!/bin/bash

# update repo
hg pull; hg update

# compile & copy
./waf production; ./waf; cp run gnomescroll_server

./shell/begin_server.sh
