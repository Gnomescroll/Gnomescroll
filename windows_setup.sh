

#copies over stuff

rm -rf ./server/c_lib/agent
cp -rf ./netclient/c_lib/agent ./server/c_lib/agent

rm -rf ./server/c_lib/common
cp -rf ./netclient/c_lib/common ./server/c_lib/common

rm -rf ./server/c_lib/net_lib
cp -rf ./netclient/c_lib/net_lib ./server/c_lib/net_lib

rm -rf ./server/c_lib/particles
cp -rf ./netclient/c_lib/particles ./server/c_lib/particles

rm -rf ./server/c_lib/physics
cp -rf ./netclient/c_lib/physics ./server/c_lib/physics

rm -rf ./server/c_lib/ray_trace
cp -rf ./netclient/c_lib/ray_trace ./server/c_lib/ray_trace

rm -rf ./server/c_lib/state
cp -rf ./netclient/c_lib/state ./server/c_lib/state

rm -rf ./server/c_lib/template
cp -rf ./netclient/c_lib/template ./server/c_lib/template

rm -rf ./server/c_lib/time
cp -rf ./netclient/c_lib/time ./server/c_lib/time

echo finished