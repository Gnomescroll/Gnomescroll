./waf clean
./waf production
./waf
version=`cat ../VERSIONX.old`
f="gnomescroll_linux32_"$version
rm -rf $f
mkdir $f
cp run $f/gnomescroll
cp *.pyc $f/
cp -R media $f/
mkdir $f/settings
cp ./settings/settings.lua $f/settings/settings.lua
cp ./settings/lua_library.lua $f/settings/lua_library.lua
mkdir $f/screenshot
mkdir $f/lib
cp /usr/lib/i386-linux-gnu/libpng12.so.0 $f/lib/
cp ../README_linux_client $f/
rm $f.tar.gz
tar cvzf $f.tar.gz $f
mv $f.tar.gz ~/Dropbox/Public
rm -rf ~/gs_build/$f
mv $f ~/gs_build/
