echo $1 > /Users/maslow/whereisit

# get path from command line, or use default
PATH=$1
if [ -z "$PATH" ]
then
    PATH="${HOME}/Desktop/Gnomescroll.app/Contents/MacOS"
fi

echo "Project dir: "
echo $PATH

echo $PATH > /Users/maslow/whereisit

# change to project dir
cd $PATH

echo "Project dir contents: "
/bin/ls "${PATH}"

# alter link lib paths
/usr/bin/install_name_tool -change "/usr/lib/libGLEW.1.7.0.dylib" "@loader_path/../Frameworks/libGLEW.1.7.0.dylib" "${PATH}"/Gnomescroll
/usr/bin/install_name_tool -change "/usr/local/lib/libluajit-51.2.dylib" "@loader_path/../Frameworks/libluajit-51.2.dylib" "${PATH}"/Gnomescroll
/usr/bin/install_name_tool -change "/Users/maslow/src/assimp--3.0.1270-source-only/lib/RelWithDebInfo/libassimp.dylib" "@loader_path/../Frameworks/libassimp.dylib" "${PATH}"/Gnomescroll

