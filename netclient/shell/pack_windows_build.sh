#!/usr/bin/env bash

./waf windows_production
./waf
version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
if [[ $version == */* ]]; then
    echo "Invalid version:" $version
    exit 1
fi
f="gnomescroll_windows_"$version
rm -rf "$f"
mkdir "$f"
cp -d run.exe "$f"/gnomescroll.exe
cp -d -R media "$f"/
mkdir "$f"/settings
cp -d ./settings/production.lua "$f"/settings/settings.lua
cp -d ./settings/lua_library.lua "$f"/settings/lua_library.lua
cp -d ./settings/load_options.lua "$f"/settings/load_options.lua
mkdir "$f"/screenshot

#mkdir -p "$f"/lib/win32/
#cp -R -d ../lib/win32/dll "$f"/lib/win32/
#cp -R -d ../lib/win32/lib "$f"/lib/win32/
#cp -R -d ../lib/win32/awesomium "$f"/lib/win32/
#rm -rf "$f"/lib/win32/awesomium/bin/debug

cp ../lib/win32/dll/* "$f"/
cp -R ../lib/win32/awesomium/bin/release/* "$f"/

if [ -d "$f".zip ]; then
    rm "$f".zip
fi

zip -r "$f".zip "$f"

# rsync the package to the server
rsync -v -e ssh "$f".zip root@m643.com:/usr/freespace/gnomescroll_downloads/"$version"/"$f".zip

# Copy to dropbox so we can get a copy to test in the vm
mv "$f".zip ~/Dropbox/Public

if [ -d "f" ]; then
    rm -rf "$f"
fi
