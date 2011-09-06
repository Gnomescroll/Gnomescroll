
#include "sequence_numbers.h"


#define INIT_V_1 0
#define INIT_V_2 0

void init_sequence_numbers(struct Pseq* ps) {
    ps->packet_sequence_number = INIT_V_1;
    int i;
    for(i=0;i<64;i++) {
        //packet_buffer[i].active = 0;
        ps->packet_sequence_buffer[i].ack = 0;
        ps->packet_sequence_buffer[i].seq = -1;
    }
}

#define UPDATE_MASK 2048-1

void process_acks(struct Pseq* ps, unsigned short seq, unsigned int flag) {
    printf("Process acks\n");
    unsigned int n = 1;
    int i;
    int index;

    index = seq;
    //index &= UPDATE_MASK;


    for(i=0;i<32;i++) {

        if((flag & n) != 0) {
            printf("+%i:%i ", index,ps->packet_sequence_buffer[index%64].seq);
        } else {
            printf("-%i:%i ", index, ps->packet_sequence_buffer[index%64].seq);
        }
        if(i%8 == 0 && i!=0) { printf("\n"); }

        index--;
        index &= UPDATE_MASK;
        n*=2;
    }
        printf("\n");


    index = seq;
    n = 1;
    for(i=0;i<32;i++) {

        if((flag & n) != 0) {
            if(ps->packet_sequence_buffer[index%64].seq != index) {printf("sequence number error 1!!!\n");}

            if(ps->packet_sequence_buffer[index%64].ack == 0) { //dont ack same packet twice
                printf("Packet Acked: %i\n", index);
                ps->packet_sequence_buffer[index%64].ack = 1;
            }

        }
    /*
        if(ps->packet_sequence_buffer[index%64].seq == index && ps->packet_sequence_buffer[index%64].ack == 0 ) {
            printf("Packet Acked: %i\n", index);
            ps->packet_sequence_buffer[index%64].ack = 1;
        }
    */
        index--;
        index &= UPDATE_MASK;
        n*=2;
    }

}

uint16_t get_next_sequence_number(struct Pseq* ps) {
    ps->packet_sequence_number = (ps->packet_sequence_number+1)%2048;
    //if(ps->packet_sequence_number >= 2048) ps->packet_sequence_number = 0;

    //printf("get_next_sequence_number: seq=%i \n", ps->packet_sequence_number);
    int index = ps->packet_sequence_number%64;

    //ps->packet_sequence_buffer[index].active = 1;
    ps->packet_sequence_buffer[index].seq = ps->packet_sequence_number;
    ps->packet_sequence_buffer[index].ack = 0;
    return ps->packet_sequence_number;
}

int DROPPED_PACKETS = 0;

int check_dropped_packets() {
    return DROPPED_PACKETS;
}

void check_for_dropped_packets(struct Pseq* ps) {
    int i,j;
    j = (ps->packet_sequence_number+1) % 64;
    for(i=0;i<32;i++) {
        if((ps->packet_sequence_buffer[j].seq != -1) && (ps->packet_sequence_buffer[j].ack == 0)) {
            printf("***Packet Dropped: %i ***\n", ps->packet_sequence_buffer[j].seq);
            DROPPED_PACKETS++;
            //ps->packet_sequence_buffer[i].ack = 0;
            ps->packet_sequence_buffer[j].seq = -1;
        }
        j= (j+1) %64;
    }
    /*
    for(i=0;i<64;i++) {
        if(i == ps->packet_sequence_number%64){
        printf("i=%i, seq=%i,ack=%i ***",i,ps->packet_sequence_buffer[i].seq,ps->packet_sequence_buffer[i].ack);
        } else {
        printf("i=%i, seq=%i,ack=%i",i,ps->packet_sequence_buffer[i].seq,ps->packet_sequence_buffer[i].ack);
        }
        printf("\n");
    }
    */
}

//

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
    pq2->seqbuff[index].seq = seq;
    /*
    if(seq > pq2->highest_packet_sequence_number){
        pq2->highest_packet_sequence_number = seq;
        //printf("new high seq: %i\n",seq);
    }
    */
    if( (seq > pq2->highest_packet_sequence_number) || ((seq < 64) && (pq2->highest_packet_sequence_number > 1984))) {
        pq2->highest_packet_sequence_number = seq;
    }
}

uint16_t get_sequence_number(struct Pseq2* pq2) {
    return pq2->highest_packet_sequence_number;
}

uint32_t generate_outgoing_ack_flag(struct Pseq2* pq2) {

    unsigned int n = 1;
    int i,index;

    uint32_t flag = 0;
    index = pq2->highest_packet_sequence_number;
    //index &= UPDATE_MASK;

    for(i=0;i<32;i++) {
        if(pq2->seqbuff[index%64].seq == index) {
            printf("+%i:%i ", pq2->seqbuff[index%64].seq, index);
            flag |= n;
        } else {
            printf("-%i:%i ", pq2->seqbuff[index%64].seq, index);
        }
        if(i%8 == 0 && i!=0) { printf("\n"); }
        index--;
        index &= UPDATE_MASK;
        n*=2;
    }

    /*
    for(i=0;i<32;i++) {
        if(pq2->seqbuff[index%64].seq == index) {
            printf("+%i:%i ", pq2->seqbuff[index%64].seq, index);
        } else {
            printf("-%i:%i ", pq2->seqbuff[index%64].seq, index);
        }
        if(i%8 == 0 && i!=0) { printf("\n"); }
        index--;
        index &= UPDATE_MASK;
    }
        printf("\n");

    for(i=0;i<64;i++) {
        if(i == pq2->highest_packet_sequence_number%64) {
            printf("i=%i, seq=%i, rec=%i ***\n",i,pq2->seqbuff[i%64].seq,pq2->seqbuff[i%64].received);
        } else {
            printf("i=%i seq=%i, rec=%i \n",i,pq2->seqbuff[i%64].seq,pq2->seqbuff[i%64].received);
        }
    }
    */
    return flag;
}
