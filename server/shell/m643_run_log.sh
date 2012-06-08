cd ..
./gnomescroll_server ./lua/settings_server_production.lua 2>&1 | tee $1
cd -
