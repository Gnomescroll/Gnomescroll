#include "pviz.h"

struct pviz_frame {
    int id;
    int start_time;
    int end_time;

    int total_acks;
    int packet_sent;
    int packet_received;
};

struct pviz_frame frame_array[128];
int frame_n = 0;

void pviz_start_frame() {
    int time = get_current_netpeer_time();
    frame_array[frame_n%128].end_time = get_current_netpeer_time();
    frame_n++;
    frame_array[frame_n%128].start_time = get_current_netpeer_time();
    frame_array[frame_n%128].total_acks = 0;
    frame_array[frame_n%128].packet_sent = 0;
    frame_array[frame_n%128].packet_received = 0;
}

/*
     glBegin(GL_POINTS);
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
    float l =0;
    while(l < distance) {
        l += delta;
        _x = x+dv.x*l;
        _y = y+dv.y*l;
        _z = z+dv.z*l;
        glVertex3f(_x,_y,_z);
    }
    glEnd();
    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
*/

#define INC_C 0.1
void pviz_draw(float x, float y, float z) {
    int ac = 0;
    int i=1;
    int index,len;
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
    glBegin(GL_POINTS);
    while(ac < 2000 && i < 128) {
        index = (frame_n - i) % 256;
        if(index < 0) {index += 256;}
        //if(frame_array[index].)
        glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
        glVertex3f(x,y-ac*INC_C,z);
        glColor3ub((unsigned char) 0,(unsigned char)255,(unsigned char)0);
        glVertex3f(x+5,y-i*3,z);
        len = frame_array[index].end_time - frame_array[index].start_time;
        ac += len;
        printf("ac= %i,i=%i len=%i\n",ac,i,len);
        i++;
    }
    printf("i=%i\n", i);
    glEnd();
    glColor3ub((unsigned char) 255,(unsigned char)255,(unsigned char)255);
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

struct pviz_packet packet_out_array[256];
//int frame_n = 0;

void pviz_packet_sent(int seq) {

    packet_out_array[seq%256].send_time = get_current_netpeer_time();
    packet_out_array[seq%256].ack_time = -1;
    packet_out_array[seq%256].send_frame = frame_n;
    packet_out_array[seq%256].ack_frame = -1;
    frame_array[frame_n%128].packet_sent++;
}

void pviz_packet_ack(int seq, int size) {
    packet_out_array[seq%256].ack_time = get_current_netpeer_time();
    packet_out_array[seq%256].ack_frame = frame_n;
    packet_out_array[seq%256].size = size;
    frame_array[frame_n%128].packet_received++;
}
