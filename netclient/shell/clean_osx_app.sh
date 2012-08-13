echo $1 > /Users/maslow/whereisit

# get path from command line, or use default
PATH=$1
if [ -z "$PATH" ]
then
    PATH="${HOME}/Desktop/Gnomescroll.app/Contents/MacOS"
fi

BIN=$2
if [ -z "$BIN" ]
then
    BIN="Gnomescroll"
fi

echo "Project dir: "
echo "${PATH}"

echo "${PATH}" > /Users/maslow/whereisit

echo "Binary name: "
echo "${$BIN}"

echo "${BIN}" > /Users/maslow/whatisit

# change to project dir
cd "${PATH}"

echo "Project dir contents: "
/bin/ls "${PATH}"


# alter link lib paths
/usr/bin/install_name_tool -change "/usr/lib/libGLEW.1.7.0.dylib" "@loader_path/../Frameworks/libGLEW.1.7.0.dylib" "${PATH}/${BIN}"
/usr/bin/install_name_tool -change "/usr/local/lib/libluajit-51.2.dylib" "@loader_path/../Frameworks/libluajit-51.2.dylib" "${PATH}/${BIN}"
/usr/bin/install_name_tool -change "/Users/maslow/src/assimp--3.0.1270-source-only/lib/RelWithDebInfo/libassimp.dylib" "@loader_path/../Frameworks/libassimp.dylib" "${PATH}/${BIN}"
/usr/bin/install_name_tool -change "/Users/maslow/src/curl-7.26.0/lib/Release/libcurl.dylib" "@loader_path/../Frameworks/libcurl.dylib" "${PATH}/${BIN}"
