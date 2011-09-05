echo "Compiling soundtest"
gcc -I. -I./api/inc sound.c testmain.c api/lib/libfmodex.so -o soundtest
