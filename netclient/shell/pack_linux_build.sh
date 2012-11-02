./waf production
./waf
version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
f="gnomescroll_linux32_"$version
rm -rf $f
mkdir $f
cp -d run $f/gnomescroll
cp -d *.pyc $f/
cp -d -R media $f/
mkdir $f/settings
cp -d ./settings/production.lua $f/settings/settings.lua
cp -d ./settings/lua_library.lua $f/settings/lua_library.lua
mkdir $f/screenshot

mkdir -p $f/lib/lin32/
cp -d ../lib/lin32/*.so $f/lib/lin32/
cp -d ../lib/lin32/*.so.* $f/lib/lin32/
cp -R -d ../lib/lin32/awesomium $f/lib/lin32/
cp -R -d ../lib/lin32/assimp $f/lib/lin32/

#cp ../notes/README_linux_client $f/README
rm $f.tar.gz
tar cvzf $f.tar.gz $f
mv $f.tar.gz ~/Dropbox/Public
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
