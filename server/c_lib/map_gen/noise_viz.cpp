

int bins[1000];

float bin_min = -0.2;
float bin_inc = 0.001;

static int less_than_min = 0;
static int greater_than_max = 0;

inline void bin_float(float x) {
    if(x < bin_min) {
        less_than_min++;
        return;
    }
    int bin = x / bin_inc;
    if(bin > 1000){
        greater_than_max++;
        return;    
    }
    bins[bin]++;
}

void setup_bin(float* arr, int n) {
    printf("Starting Binning\n");
    int i;
    for(i=0; i<1000;i++) bins[i] = 0;
    double average = 0; 
    for(i=0; i<n; i++) {
        average += arr[n];
        bin_float(arr[n]);
    }
    average = average / (double)(n);
    printf("average= %d \n", average);
    printf("less_than_min= %i \n",less_than_min);
    printf("greater_than_max= %i \n", greater_than_max);
    printf("finished binning\n");

}

#ifdef DC_CLIENT
void bin_histrogram_draw(float x, float y, float z) {
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
