
gcc -Wall -g -I../ test_server.c ../time/physics_timer.c -o test_server -lrt
gcc -Wall -g -I../ test_client.c ../time/physics_timer.c -o test_client -lrt
