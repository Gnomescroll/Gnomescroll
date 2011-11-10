
./py2exe_package.py py2exe

rm -rf ./dist/content
cp -rf ./content ./dist/content

rm -rf ./dist/ext
cp -rf ./ext ./dist/ext

rm -rf ./dist/dats
cp -rf ./dats ./dist/dats

cp -f ./c_lib.dll ./dist/c_lib.dll