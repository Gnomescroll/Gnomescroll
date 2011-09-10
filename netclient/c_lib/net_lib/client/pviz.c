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
    frame_array[frame_n%128].end_time = -1;
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

//1028x800
void pviz_draw_grid(float z) {
    return;
    //printf("X");'
    z = -0.9;
    float xstep = 10;
    float ystep = 50;
    int i,j;
    //glEnable(GL_POINT_SMOOTH);
    //glPointSize(1.00);
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)255);
    glBegin(GL_POINTS);
    for(i=0; i<50;i++) {
    for(j=0; j<50;j++) {
        //printf("drew");
       glVertex3f((float)i*xstep+0.5,(float)j*ystep+0.5,z);
       //glVertex2i(i*xstep, j*ystep);
    }
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}

#define INC_C 0.5
void pviz_draw(float x, float y, float z) {

    if(x==0) {
        pviz_draw_grid(z);
        return;
    }

    int ac = 0;
    int i=1;
    int index,len;
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
    glBegin(GL_POINTS);


    float d = 10;

    glColor3ub((unsigned char) 0,(unsigned char)255,(unsigned char)255);
    glVertex3f(x,y,z);
    glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
    glVertex3f(x+d,y,z);
    glVertex3f(x-d,y,z);
    glVertex3f(x,y+d,z);
    glVertex3f(x,y-d,z);
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)255);
    glVertex3f(x,y,z+d);
    glVertex3f(x,y,z-d);

    while(ac*INC_C < 800 && i < 32) {
        index = (frame_n - i) % 128;
        if(index < 0) {index += 128;}
        //if(frame_array[index].)
        glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
        glVertex3f(x,y-ac*INC_C,z);
        glColor3ub((unsigned char) 0,(unsigned char)255,(unsigned char)0);
        glVertex3f(x+5,y-i*3,z);
        len = frame_array[index].end_time - frame_array[index].start_time;
        ac += len;
        //printf("index=%i ac=%i i=%i len=%i\n",index, ac,i,len);
        i++;
    }
    //printf("i=%i\n", i);
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
