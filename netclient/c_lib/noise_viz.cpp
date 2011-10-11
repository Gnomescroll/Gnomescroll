#include <c_lib/noise_viz.hpp>
//srand((unsigned)time(0));

static const int num_bins = 1000;
static int bin2[num_bins];

const float bin_min = 0;
const float bin_inc = 0.001;

float bin_scale;

static int less_than_min = 0;
static int greater_than_max = 0;

void noise_viz_test_setup() {
    
    int i;
    float test[2000];
    for(i=0; i< 2000; i++) {
    test[i] = (float)rand()/(float)RAND_MAX;
    }
}
inline void bin_float(float x) {
    if(x < bin_min) {
        less_than_min++;
        return;
    }
    int index = (x-bin_min) / bin_inc;
    if(index > num_bins){
        greater_than_max++;
        return;    
    }
    bin2[index]++;
}

void setup_bin(float* arr, int n) {
    printf("Starting Binning\n");
    int i;
    for(i=0; i<1000;i++) bin2[i] = 0;
    double average = 0; 
    for(i=0; i<n; i++) {
        average += arr[n];
        bin_float(arr[n]);
    }
    average = average / (double)(n);
    printf("number of samples= %i\n", n);
    printf("average= %f \n", average);
    printf("less_than_min= %i \n",less_than_min);
    printf("greater_than_max= %i \n", greater_than_max);
    printf("finished binning\n");

    bin_scale = ((float)(n))/1000;
}


void rmf_dist(float* arr, int x, int y, int z) {
    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    float fz = (float)z + 2.0f;
    
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                arr[i + x*j + x*y*k] = rmf_perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x, y, z, 0);
            }
        }
    }
}

#ifdef DC_CLIENT
void plot_rmf(int x, int y, int z) {

    float *_noise = (float*) malloc(sizeof(float)*x*y*z);
    rmf_dist(_noise, x, y, z);
    setup_bin(_noise, x*y*z);
    bin_histrogram_draw(100.0f, 100.0f, 0.0f);
    free(_noise);
}
#endif


#define _C 0.5
#ifdef DC_CLIENT
void draw_noise_viz_histrogram(float x, float y, float z) {
    //printf("po=%i\n", PO_L);
    x -= 10;
    y += 10;
    int i,j;

    glBegin(GL_POINTS);
    glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
    for(i=0; i<num_bins; i++) {
        //printf("bin[%i]=%i\n", i,bin[i]);
        for(j=0; j<bin2[i]; j++) {
            glVertex3f(_C+x+2*j,_C+y+2*i,z);
        }
    }

    glColor3ub((unsigned char) 200,(unsigned char)0,(unsigned char)0);
    for(i=0; i<8; i++) {
        glVertex3f(_C+x-2,_C+y+2*10*i,z);
        glVertex3f(_C+x-3,_C+y+2*10*i,z);
        glVertex3f(_C+x-4,_C+y+2*10*i,z);
    }
    glEnd();

    //printf("time= %i\n", get_current_netpeer_time());
}
#endif
