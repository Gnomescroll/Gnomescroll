
#include "sequence_numbers.h"

/*
struct packet_sequence {
    int seq;
    int ack;
    //int active;
    //int tick; //better than sequence for determining dropped packets
};


struct packet_sequencer() {
    struct packet_sequence packet_sequence_buffer[64];
    int packet_sequence_buffer_n = 0;
}
*/
//unsigned int packet_buffer[32];

//struct packet_sequence packet_sequence_buffer[64];
//int ps->packet_sequence_number = 0;

#define INIT_V_1 0
#define INIT_V_2 0

void init_sequence_numbers(struct Pseq* ps) {
    ps->packet_sequence_number = 0;
    int i;
    for(i=0;i<64;i++) {
        //packet_buffer[i].active = 0;
        ps->packet_sequence_buffer[i].ack = 0;
        ps->packet_sequence_buffer[i].seq = -1;
    }
}

void process_acks(struct Pseq* ps, unsigned short seq, unsigned int flag) {
    printf("Process acks\n");
    unsigned int n = 1;
    int i,j;
    int index;
    for(i=0;i<32;i++) {
        index = (seq -i) % 2048;// % 2048; //seq is id of highest packet server has seen yet
        if(index < 0) index+=2048;

        if(flag & (n != 0)) {
            //ack that packet
            j = seq%64;
                if(ps->packet_sequence_buffer[j].seq == index) {
                    if(ps->packet_sequence_buffer[j].ack == 0) {
                        printf("Packet Acked: %i\n", index);
                    }
                    ps->packet_sequence_buffer[j].ack = 1;
                    //printf("Packet Acked: %i\n", index);
                }
            }
        n*=2;
    }
}

uint16_t get_next_sequence_number(struct Pseq* ps) {
    ps->packet_sequence_number += 1;
    if(ps->packet_sequence_number >= 2048) ps->packet_sequence_number = 0;
    //printf("get_next_sequence_number: seq=%i \n", ps->packet_sequence_number);
    int index = ps->packet_sequence_number%64;

    //ps->packet_sequence_buffer[index].active = 1;
    ps->packet_sequence_buffer[index].seq = ps->packet_sequence_number;
    ps->packet_sequence_buffer[index].ack = 0;
    return ps->packet_sequence_number;
}

void check_for_dropped_packets(struct Pseq* ps) {
    int i,j; //,j;
    //int index;
    //j = (ps->packet_sequence_number%64);
    //if(j<0) j+=64;
    //index = (ps->packet_sequence_number % 2048);
    //if(index < 0) index+= 2048;
    //j = (ps->packet_sequence_number+1) % 64;


    for(i=0;i<64;i++) {
        if(i == ps->packet_sequence_number%64){
        printf("i=%i, seq=%i,ack=%i ***",i,ps->packet_sequence_buffer[i].seq,ps->packet_sequence_buffer[i].ack);
        } else {
        printf("i=%i, seq=%i,ack=%i",i,ps->packet_sequence_buffer[i].seq,ps->packet_sequence_buffer[i].ack);
        }
        printf("\n");
    }

    j = (ps->packet_sequence_number+1) % 64;
    for(i=0;i<32;i++) {


    if((ps->packet_sequence_buffer[j].seq != -1) && (ps->packet_sequence_buffer[j].ack == 0)) {
        printf("***Packet Dropped: %i i=%i ***\n", ps->packet_sequence_buffer[j].seq,j);
        //ps->packet_sequence_buffer[i].ack = 0;
        ps->packet_sequence_buffer[j].seq = -1;
    }
    j= (j+1) %64;
    }
        //if(ps->packet_sequence_buffer[i].seq == index) {
        //    printf("equals\n");
        //}
        //use tick based
    /*
        if(ps->packet_sequence_buffer[i].seq >=0 && ps->packet_sequence_number - ps->packet_sequence_buffer[i].seq > 32) {
            printf("Packet %i assumed lost\n", ps->packet_sequence_buffer[i].seq);
            ps->packet_sequence_buffer[i].seq = -1;
        }
    */
        //j = (j+1) %64;
        //index = (index+1)%64;

    printf("from %i to %i\n", (ps->packet_sequence_number+1) % 64, (ps->packet_sequence_number+1+32) % 64);
    //printf("*/n*/n*/n");
}

//

/*
struct Pseq2 {
    struct packet_sequence2 packet_sequence_buffer[64];
    int highest_packet_sequence_number;
};

void set_ack_for_received_packet(struct Pseq2* pq2);
uint32_t generate_outgoing_ack_flag(struct Pseq2* pq2);
*/

void init_sequence_numbers_out(struct Pseq2* pq2) {
    int i;
    pq2->highest_packet_sequence_number = INIT_V_2;
    for(i=0;i<64;i++) {
    pq2->seqbuff[i].seq = -1;
    pq2->seqbuff[i].received = 0;
    }
}

void set_ack_for_received_packet(struct Pseq2* pq2, int seq) {
    int index = seq % 64;
    pq2->seqbuff[index].received = 1;
    if(seq > pq2->highest_packet_sequence_number){
        pq2->highest_packet_sequence_number = seq;
        //printf("new high seq: %i\n",seq);
    } else {
        //printf("set_ack_for_received_packet\n");
    }
}

uint16_t get_sequence_number(struct Pseq2* pq2) {
    return pq2->highest_packet_sequence_number;
}

uint32_t generate_outgoing_ack_flag(struct Pseq2* pq2) {

    unsigned int n = 1;
    int i,index;

    uint32_t flag = 0;
    for(i=0;i<32;i++) { //should start at 1
        index = (pq2->highest_packet_sequence_number -i) % 2048;// % 2048; //seq is id of highest packet server has seen yet
        if(index < 0) index+=2048;

        if(pq2->seqbuff[index%64].received == 1) {
            flag |= n;
        }
        n*=2;
    }
    return flag;
}
