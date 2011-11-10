
./py2exe_package.py py2exe

rm -rf ./dist/ext
cp -rf ./ext ./dist/ext

rm -rf ./dist/media
cp -rf ./media ./dist/media

cp -f ./c_lib.dll ./dist/c_lib.dll