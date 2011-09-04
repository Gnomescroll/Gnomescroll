echo "Compiling soundtest"
gcc -I. -I./api/inc sound.c player_sounds.c testmain.c api/lib/libfmodex.so -o soundtest
