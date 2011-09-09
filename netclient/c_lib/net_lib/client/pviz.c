#include "pviz.h"

struct pviz_frame {
    int id;
    int start_time;
    int end_time;

    int total_acks;
    int total_packets;
};

struct pviz_frame frame_array[128];
int frame_n = 0;

void pviz_start_frame() {
    int time = get_current_netpeer_time();
    frame_array[frame_n%128].end_time = get_current_netpeer_time();
    frame_n++;
    frame_array[frame_n%128].start_time = get_current_netpeer_time();
    frame_array[frame_n%128].total_acks = 0;
    frame_array[frame_n%128].total_packets = 0;
}


void pviz_draw(float x, float y, float z) {

}

struct pviz_packet {
    int seq;
    int send_time;
    int ack_time;
    int send_frame;
    int ack_frame;

    int size;
    int total_packets;
};

struct pviz_packet packet_array[256];
int frame_n = 0;

void pviz_packet_send(int seq) {

    packet_array[seq%256].send_time = get_current_netpeer_time();
    packet_array[seq%256].ack_time = -1;
    packet_array[seq%256].send_frame = frame_n;
    packet_array[seq%256].ack_frame = -1
}

void pviz_packet_ack(int seq) {



}
