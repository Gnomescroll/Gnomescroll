#!/usr/bin/env bash

if [ -z "$1" ]; then
    arch="32"
else
    arch="$1"
fi

if [ $arch != "32" ] && [ $arch != "64" ]; then
    echo "Unknown architecture:" $arch
    exit
fi

./waf production
./waf
version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
f="gnomescroll_linux"$arch"_"$version

if [ -d "$f" ]; then
    rm -rf $f
fi

mkdir "$f"
cp -d run "$f"/gnomescroll
cp -d -R media "$f"/
mkdir "$f"/settings
cp -d ./settings/production.lua "$f"/settings/settings.lua
cp -d ./settings/lua_library.lua "$f"/settings/lua_library.lua
cp -d ./settings/load_options.lua "$f"/settings/load_options.lua
mkdir "$f"/screenshot

mkdir -p "$f"/lib/lin$arch/
cp -d ../lib/lin$arch/*.so "$f"/lib/lin$arch/
cp -d ../lib/lin$arch/*.so.* "$f"/lib/lin$arch/
cp -R -d ../lib/lin$arch/awesomium "$f"/lib/lin$arch/

# Remove any existing package
rm "$f".tar.gz
# Create package
tar cvzf "$f".tar.gz "$f"

# Remove any existing local saved copy of the folder
mkdir -p ~/gs_build/
rm -rf ~/gs_build/"$f"
# Move the folder to our stash
mv "$f" ~/gs_build/

# rsync the package to the server
rsync -v -e ssh "$f".tar.gz root@m643.com:/usr/freespace/gnomescroll_downloads/"$version"/"$f".tar.gz
