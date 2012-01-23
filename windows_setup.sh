

#copies over stuff

#netlib
rm -rf ./server/net_lib
cp -rf ./netclient/net_lib ./server/net_lib

#other stuff
rm -rf ./server/c_lib/agent
cp -rf ./netclient/c_lib/agent ./server/c_lib/agent

rm -rf ./server/c_lib/common
cp -rf ./netclient/c_lib/common ./server/c_lib/common

rm -rf ./server/c_lib/game
cp -rf ./netclient/c_lib/game ./server/c_lib/game

rm -rf ./server/c_lib/particles
cp -rf ./netclient/c_lib/particles ./server/c_lib/particles

rm -rf ./server/c_lib/physics
cp -rf ./netclient/c_lib/physics ./server/c_lib/physics

rm -rf ./server/c_lib/items
cp -rf ./netclient/c_lib/items ./server/c_lib/items

rm -rf ./server/c_lib/monsters
cp -rf ./netclient/c_lib/monsters ./server/c_lib/monsters

rm -rf ./server/c_lib/ray_trace
cp -rf ./netclient/c_lib/ray_trace ./server/c_lib/ray_trace

rm -rf ./server/c_lib/state
cp -rf ./netclient/c_lib/state ./server/c_lib/state

rm -rf ./server/c_lib/template
cp -rf ./netclient/c_lib/template ./server/c_lib/template

rm -rf ./server/c_lib/time
cp -rf ./netclient/c_lib/time ./server/c_lib/time

rm -rf ./server/c_lib/voxel
cp -rf ./netclient/c_lib/voxel ./server/c_lib/voxel

rm -rf ./server/c_lib/weapons
cp -rf ./netclient/c_lib/weapons ./server/c_lib/weapons

echo finished
