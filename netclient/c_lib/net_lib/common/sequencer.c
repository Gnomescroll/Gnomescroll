
#include "sequencer.h"


#define INIT_V_1 0
#define INIT_V_2 0


void init_sequencer(struct NetPeer* np) {
    init_sequence_numbers(np);
    init_sequence_numbers_out(np);
}

void init_sequence_numbers(struct NetPeer* np) {
    np->packet_sequence_number = INIT_V_1;
    int i;
    for(i=0;i<256;i++) {
        //packet_buffer[i].active = 0;
        np->packet_sequence_buffer[i].ack = 0;
        np->packet_sequence_buffer[i].seq = -1;
    }
}

#define UPDATE_MASK 2048-1

void process_acks(struct NetPeer* np, unsigned short seq, unsigned int flag) {
    unsigned int n = 1;
    int i;
    int index;

    //index &= UPDATE_MASK;

    /*
    n = 1;     index = seq;
    for(i=0;i<32;i++) {
        if((flag & n) != 0) {printf("+%i:%i ", index,np->packet_sequence_buffer[index%256].seq);}
        else { printf("-%i:%i ", index, np->packet_sequence_buffer[index%256].seq);}
        if(i%8 == 0 && i!=0) { printf("\n"); }
        index--;index &= UPDATE_MASK;n*=2;
    }
    printf("\n");
    */
    index = seq;
    n = 1;
    for(i=0;i<32;i++) {

        if((flag & n) != 0) {
            if(np->packet_sequence_buffer[index%256].seq != index) {
                printf("sequence number error: expected %i, received %i\n", np->packet_sequence_buffer[index%256].seq, index);
                printf("i=%i, seq=%i, seq256=%i\n", i, seq, seq%256);

                }

            if(np->packet_sequence_buffer[index%256].ack == 0) { //dont ack same packet twice
                //printf("Packet Acked: %i:%i\n", np->client_id,index);
                np->packet_sequence_buffer[index%256].ack = 1;
            }

        }
    /*
        if(np->packet_sequence_buffer[index%256].seq == index && np->packet_sequence_buffer[index%256].ack == 0 ) {
            printf("Packet Acked: %i\n", index);
            np->packet_sequence_buffer[index%256].ack = 1;
        }
    */
        index--;
        index &= UPDATE_MASK;
        n*=2;
    }

}

uint16_t get_next_sequence_number(struct NetPeer* np) {
    np->packet_sequence_number = (np->packet_sequence_number+1)%2048;
    int index = np->packet_sequence_number%256;
    //np->packet_sequence_buffer[index].active = 1; //may set timer?
    np->packet_sequence_buffer[index].seq = np->packet_sequence_number;
    np->packet_sequence_buffer[index].ack = 0;
    return np->packet_sequence_number;
}

int DROPPED_PACKETS = 0;

int check_dropped_packets() {
    return DROPPED_PACKETS;
}

void check_for_dropped_packets(struct NetPeer* np) {
    int i,j;
    j = (np->packet_sequence_number+1) % 256;
    for(i=0;i<32;i++) {
        if((np->packet_sequence_buffer[j].seq != -1) && (np->packet_sequence_buffer[j].ack == 0)) {
            printf("***Packet Dropped: %i:%i ***\n", np->client_id, np->packet_sequence_buffer[j].seq);
            DROPPED_PACKETS++;
            //np->packet_sequence_buffer[i].ack = 0;
            np->packet_sequence_buffer[j].seq = -1;
        }
        j= (j+1) %256;
    }
    /*
    for(i=0;i<256;i++) {
        if(i == np->packet_sequence_number%256){
        printf("i=%i, seq=%i,ack=%i ***",i,np->packet_sequence_buffer[i].seq,np->packet_sequence_buffer[i].ack);
        } else {
        printf("i=%i, seq=%i,ack=%i",i,np->packet_sequence_buffer[i].seq,np->packet_sequence_buffer[i].ack);
        }
        printf("\n");
    }
    */
}

//

void init_sequence_numbers_out(struct NetPeer* np) {
    int i;
    np->highest_packet_sequence_number = INIT_V_2;
    for(i=0;i<256;i++) {
        np->seqbuff[i].seq = -1;
        np->seqbuff[i].received = 0;
    }
}

void set_ack_for_received_packet(struct NetPeer* np, int seq) {
    int index = seq % 256;
    np->seqbuff[index].received = 1;
    np->seqbuff[index].seq = seq;
    /*
    if(seq > np->highest_packet_sequence_number){
        np->highest_packet_sequence_number = seq;
        //printf("new high seq: %i\n",seq);
    }
    */
    if( (seq > np->highest_packet_sequence_number) || ((seq < 256) && (np->highest_packet_sequence_number > 1984))) {
        np->highest_packet_sequence_number = seq;
    }
}

uint16_t get_sequence_number(struct NetPeer* np) {
    return np->highest_packet_sequence_number;
}

uint32_t generate_outgoing_ack_flag(struct NetPeer* np) {

    unsigned int n = 1;
    int i,index;

    uint32_t flag = 0;
    index = np->highest_packet_sequence_number;
    //index &= UPDATE_MASK;

    for(i=0;i<32;i++) {
        if(np->seqbuff[index%256].seq == index) flag |= n;
        index--;
        index &= UPDATE_MASK;
        n*=2;
    }

    /*
    for(i=0;i<32;i++) {
        if(np->seqbuff[index%256].seq == index) {
            printf("+%i:%i ", np->seqbuff[index%256].seq, index);
        } else {
            printf("-%i:%i ", np->seqbuff[index%256].seq, index);
        }
        if(i%8 == 0 && i!=0) { printf("\n"); }
        index--;
        index &= UPDATE_MASK;
    }
        printf("\n");

    for(i=0;i<256;i++) {
        if(i == np->highest_packet_sequence_number%256) {
            printf("i=%i, seq=%i, rec=%i ***\n",i,np->seqbuff[i%256].seq,np->seqbuff[i%256].received);
        } else {
            printf("i=%i seq=%i, rec=%i \n",i,np->seqbuff[i%256].seq,np->seqbuff[i%256].received);
        }
    }
    */
    return flag;
}
