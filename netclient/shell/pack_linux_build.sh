./waf clean
./waf production
./waf
version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
f="gnomescroll_linux32_"$version
rm -rf $f
mkdir $f
cp run $f/gnomescroll
cp *.pyc $f/
cp -R media $f/
mkdir $f/settings
cp ./settings/production.lua $f/settings/settings.lua
cp ./settings/lua_library.lua $f/settings/lua_library.lua
mkdir $f/screenshot

mkdir $f/lib
cp ../lib/lin32/*.so $f/lib
cp ../lib/lin32/*.so.* $f/lib
mkdir $f/lib/awesomium/lib/
cp ../lib/lin32/awesomium/lib/* $f/lib/awesomium/lib/
mkdir $f/lib/assimp/lib/
cp ../lib/lin32/assimp/lib/* $f/lib/assimp/lib/

#cp ../notes/README_linux_client $f/README
rm $f.tar.gz
tar cvzf $f.tar.gz $f
mv $f.tar.gz ~/Dropbox/Public
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
