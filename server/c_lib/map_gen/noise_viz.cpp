

int bins[1000];

float bin_min;
float bin


inline void bin_float(float x) {
    

}

void setup_bin(float* arr, int n) {
    int i;
    double average = 0; 
    for(i=0; i<n; i++) {
        average += arr[n];
        bin_float(arr[n]);
    }
    average = average / (double)(n);
    prinf("average= %d", average);

}
#ifdef DC_CLIENT
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

    glColor3ub((unsigned char) 00,(unsigned char)0,(unsigned char)200);
    for(i=0; i<8; i++) {
        glVertex3f(_C+x-2,_C+y+2*10*i,z);
        glVertex3f(_C+x-3,_C+y+2*10*i,z);
        glVertex3f(_C+x-4,_C+y+2*10*i,z);
    }
    glEnd();

    //printf("time= %i\n", get_current_netpeer_time());
}
#endif
