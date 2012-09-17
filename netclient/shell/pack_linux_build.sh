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
cp -R ../lib/lin32 $f/lib/
#cp ../notes/README_linux_client $f/README
rm $f.tar.gz
tar cvzf $f.tar.gz $f
mv $f.tar.gz ~/Dropbox/Public
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
