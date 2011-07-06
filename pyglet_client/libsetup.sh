cd ./netclient
echo 'Compiling Libs'

echo "Compiling Binaries"

gcc main.c -I/usr/include/python2.6/ -L/usr/include/python2.6 -lpython2.6 -o main
gcc run.c -I/usr/include/python2.6/ -L/usr/include/python2.6 -lpython2.6 -o run

echo "Compiling Libs"

g++ -Wall -fPIC -c libChrome.c -I/usr/local/include -I /usr/include/X11/extensions/ -I/usr/include/SDL -I/usr/lib -I./include/ -I./ -o libChrome.o

g++ -shared -o libChrome.so libChrome.o -I./include/ -L. -lAwesomium -Wl,-rpath=./

cython ChromeEmbedded.pyx

gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/local/include -I /usr/include/X11/extensions/ -I/usr/include/SDL -I/usr/lib -I./include/ -I./ -I/usr/include/python2.6 -c ChromeEmbedded.c -o ChromeEmbedded.o -Wl,-rpath=./ -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -g

gcc -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions ChromeEmbedded.o -L./ -L/usr/X11R6/lib -Lusr/lib -lSDL -lGL -lSDL -lGLU -lSDL_image -lChrome -o ChromeEmbedded.so -Wl,-rpath=./ -g
