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
    frame_n = frame_n +1;
    frame_array[frame_n%128].start_time = get_current_netpeer_time();
    frame_array[frame_n%128].end_time = -1;
    frame_array[frame_n%128].total_acks = 0;
    frame_array[frame_n%128].packet_sent = 0;
    frame_array[frame_n%128].packet_received = 0;
    //printf("frame=%i\n", frame_n);
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
    pviz_packet_histrogram_draw(x,y,z);
    pviz_packet_histrogram2_draw(x,y,z);

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
    float _d = 0.5;
    glColor3ub((unsigned char) 0,(unsigned char)255,(unsigned char)255);
    glVertex3f(_d+x,_d+y,z);
    glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
    glVertex3f(_d+x+d,_d+y,z);
    glVertex3f(_d+x-d,_d+y,z);
    glVertex3f(_d+x,_d+y+d,z);
    glVertex3f(_d+x,_d+y-d,z);
    glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)255);
    glVertex3f(_d+x,_d+y,z);
    /*
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
    */
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

#define PO_L 256  //number of packets to keep

struct pviz_packet packet_out_array[PO_L];
//int frame_n = 0;

void pviz_packet_sent(int seq, int size) {

    packet_out_array[seq%PO_L].send_time = get_current_netpeer_time();
    packet_out_array[seq%PO_L].ack_time = -1;
    packet_out_array[seq%PO_L].send_frame = frame_n;
    packet_out_array[seq%PO_L].ack_frame = -1;
    packet_out_array[seq%PO_L].size = size;
    frame_array[frame_n%128].packet_sent++;
}

void pviz_packet_ack(int seq) {
    packet_out_array[seq%PO_L].ack_time = get_current_netpeer_time();
    packet_out_array[seq%PO_L].ack_frame = frame_n;
    frame_array[frame_n%128].packet_received++;
}

#define _C 0.5
//yellow, sent but un acked
//red send but presumed dropped; unpacked, send more than 1000ms ago
//green acked

#define PO_L2 64 // number to display on screen
void pviz_packet_histrogram_draw(float x, float y, float z) {
    //printf("Test\n");
    x -= 10.0;
    y -= 10.0;
    int time = get_current_netpeer_time();
    int i,j,k;
    int ft;



    int ac = 2.0;
    glPointSize(2.0);
    glBegin(GL_POINTS);

    i = frame_n % PO_L;
    for(k=0; k<PO_L2; k++) {
        if(packet_out_array[i].ack_frame == -1) {
            if(time - packet_out_array[i].send_time > 500) {
                glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
            } else {
                glColor3ub((unsigned char) 153,(unsigned char)50,(unsigned char)204);
            }
            glVertex3f(_C+x,_C+y-2*ac*k,z);
        } else {
            glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
            ft = packet_out_array[i].ack_frame - packet_out_array[i].send_frame;
            //printf("ft=%i i=%i ack_t=%i send_t=%i\n", ft, i,packet_out_array[i].ack_frame,packet_out_array[i].send_frame );
            for(j=0; j<ft; j++) {
                glVertex3f(_C+x+3*j,_C+y-2*ac*k,z);
            }
        }
        i--;
        if(i<0) i = PO_L2;
    }
    glEnd();
    glPointSize(1.0);


/*
    int ac = 2.0;
    glPointSize(1.0);
    glBegin(GL_POINTS);

    for(i=0; i<PO_L; i++) {
        if(packet_out_array[i].ack_frame == -1) {
            if(time - packet_out_array[i].send_time > 500) {
                glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
            } else {
                glColor3ub((unsigned char) 153,(unsigned char)50,(unsigned char)204);
            }
            glVertex3f(_C+x,_C+y-ac*i,z);
        } else {
            glColor3ub((unsigned char) 0,(unsigned char)255,(unsigned char)0);
            ft = packet_out_array[i].ack_frame - packet_out_array[i].send_frame;
            //printf("ft=%i i=%i ack_t=%i send_t=%i\n", ft, i,packet_out_array[i].ack_frame,packet_out_array[i].send_frame );
            for(j=0; j<ft; j++) {
                glVertex3f(_C+x+j,_C+y-ac*i,z);
            }
        }
    }
    glEnd();
*/
    /*
    glPointSize(2.0);
    glBegin(GL_POINTS);
    for(i=0; i<PO_L; i++) {
        if(packet_out_array[i].ack_frame == -1) {
            if(time - packet_out_array[i].send_time > 500) {
                glColor3ub((unsigned char) 255,(unsigned char)0,(unsigned char)0);
            } else {
                glColor3ub((unsigned char) 153,(unsigned char)50,(unsigned char)204);
            }
            glVertex3f(_C+x,_C+y-2*ac*i,z);
        } else {
            glColor3ub((unsigned char) 0,(unsigned char)255,(unsigned char)0);
            ft = packet_out_array[i].ack_frame - packet_out_array[i].send_frame;
            //printf("ft=%i i=%i ack_t=%i send_t=%i\n", ft, i,packet_out_array[i].ack_frame,packet_out_array[i].send_frame );
            for(j=0; j<ft; j++) {
                glVertex3f(_C+x+3*j,_C+y-2*ac*i,z);
            }
        }
    }
    glEnd();
    */
}

//#define bin_size 8

int bin_size = 1;
#define num_bins 512
int bin[num_bins];

void toggle_latency_unit() {
    if(bin_size==1) bin_size =2;
    else if(bin_size==2) bin_size =5;
    else if(bin_size==5) bin_size =10;
    else if(bin_size==10) bin_size =1;
    printf("pviz_packet_histrogram2: latency unit = %ims\n", bin_size);
}

void pviz_packet_histrogram2_draw(float x, float y, float z) {
    //printf("po=%i\n", PO_L);
    x -= 10;
    y += 10;
    int i,j;
    for(i=0;i<num_bins;i++) bin[i]=0;
    int ft;
    for(i=0; i<PO_L; i++) {
        if(packet_out_array[i].ack_frame == -1) continue;
        ft = packet_out_array[i].ack_time - packet_out_array[i].send_time;
        //printf("ft=%i\n", ft);
        ft = ft / bin_size;

        if(ft <= 0 || ft >= num_bins) {
            //printf("ft error: ft= %i\n", ft);
            continue;
        }
        bin[ft]++;

    }

    glBegin(GL_POINTS);
    glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
    for(i=0; i<num_bins; i++) {
        //printf("bin[%i]=%i\n", i,bin[i]);
        for(j=0; j<bin[i]; j++) {
            glVertex3f(_C+x+2*j,_C+y+2*i,z);
        }
    }
    glEnd();

    //printf("time= %i\n", get_current_netpeer_time());
}
