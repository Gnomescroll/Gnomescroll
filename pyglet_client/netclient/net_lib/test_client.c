
#include "net_packets.h"
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "client/client2.c"


/*

void init_client();

void send_packet(unsigned char* buffer, int n);

void attempt_connection_with_server();

void set_server(int a, int b, int c, int d, unsigned short port);
int validate_packet(unsigned char* buffer, int n, struct sockaddr_in*);
void process_incoming_packets();
void process_packet(unsigned char* buffer, int n);
void process_outgoing_packets();
*/


int main() {

    init_client();

    unsigned int port = 9999;
    set_server(127,0,0,1, port);
    attempt_connection_with_server();

    struct Pseq* ps = CLIENT_get_Pseq();

    sleep(1);

    int i=0;
    while(1) {

        printf("* ------ tick=%i\n", i);

        process_incoming_packets();
        send_packet2();
        send_packet2();
    /*
        send_packet2();
        send_packet2();
        send_packet2();
        send_packet2();
    */

        check_for_dropped_packets(ps);
        sleep(1);
        i++;
        }
    return 0;
}
/*
    unsigned char* buffer[6];
    int n=0;
    PACK_uint16_t(0, buffer, &n)
    PACK_uint8_t(255, buffer, &n)

    struct Socket* s;
    s = create_socket(0, 9091);
    sleep(1);
    struct NetPeer* p;
    p = create_net_peer(127,0,0,1, 9999);

    printf("send1\n");
    unsigned char data1[4] = {0,0,255,255};
    send_packet(s,p, data1, 4);

    printf("send2\n");
    unsigned char data2[6] = "aaabbb";

    //data2 = *((uint16_t *) packet_data)

    send_packet(s,p, data2, 6);

    //receive_packets(s);
*/
