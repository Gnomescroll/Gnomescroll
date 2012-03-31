

rm -rf ./dist

mkdir ./dist
#mkdir ./dist/netclient
#mkdir ./dist/server

cp -f ./server/cube.py ./netclient/cube_dat.py

cd netclient
./win_package.sh
cd ..
cp -rf ./netclient/dist/. ./dist/

#cd server
#./win_package.sh
#cd ..
#cp -rf ./server/dist/. ./dist/server/

rm -rf /c/Dropbox/gnomescroll_
cp -rf ./dist/ /c/Dropbox/gnomescroll_
