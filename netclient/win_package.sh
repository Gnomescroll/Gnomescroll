
./py2exe_package.py py2exe

cp -f ./c_lib.dll ./dist/c_lib.dll

rm -f ./dist/settings.py
cp -f ./settings.py ./dist/settings.py

rm -rf ./dist/ext
cp -rf ./ext ./dist/ext

rm -rf ./dist/media
cp -rf ./media ./dist/media

cp -f ./c_lib.dll ./dist/c_lib.dll