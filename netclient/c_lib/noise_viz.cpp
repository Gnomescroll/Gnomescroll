#include <c_lib/noise_viz.hpp>
//srand((unsigned)time(0));

static const int num_bins = 400;
static int bin2[num_bins];

const float bin_min = -1.0;
const float bin_inc = 0.005;

float bin_scale;

static int less_than_min = 0;
static int greater_than_max = 0;

float noise_viz_test[2000];
void noise_viz_test_setup() {
    
    int i;

    for(i=0; i< 2000; i++) {
        noise_viz_test[i] = (float)rand()/(float)RAND_MAX;
        //printf("%f \n", noise_viz_test[i]);
    }
/*
    for(i=0; i< 2000; i++) {
        printf("%i= %f \n", i, noise_viz_test[i]);
    }
*/
    noise_viz_setup_bin(noise_viz_test, 2000);
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

float _bin_min = 0.5;
float _bin_inc = 0.005;
float _bin_range;

float percentile_cutoff_calculation(float percentile, float* arr, int n) {
    int i;
    printf("=== Start percentile_cutoff_calculation ===\n");
    const int _num_bins = 1000; //parameter
    float* bin_array = (float*) malloc(sizeof(float)*_num_bins);
    for(i=0; i<_num_bins;i++) bin_array[i] = 0;

    double average = 0;   
    float min =  100000;
    float max = -100000;
    for(i=0; i<n; i++) {
        if(arr[i] < min) min = arr[i];
        if(arr[i] > max) max = arr[i];
        average += arr[i];
    }
    printf("number of samples= %i\n", n);
    printf("average= %f, sum= %f \n", average/n, average);
    printf("min_value= %f \n", min);
    printf("max_value= %f \n", max);

    float _bin_range = max - min;
    float _bin_inc = _bin_range / _num_bins;

    //do binning
    int bin_index;
    for(i=0; i<n; i++) {
        bin_index = (arr[i]-min) / bin_inc;
        bin_array[bin_index]++;
    }
    //do percentile calculation
    int counter = 0;
    int terminal = percentile* ((float)(n));
    float cutoff = -1;
    for(i=0; i<_num_bins; i++) {
        counter += bin_array[i];
        if(counter > terminal) {
            cutoff = min+i*_bin_inc;
            printf("percentile_cuttoff: percentile= %f, cutoff= %f \n", percentile, cutoff);
            break;
        }
    }
/*
    for(i=0; i<_num_bins; i++) {
        counter += bin_array[i];
        if(counter > terminal) {
            cutoff = min+i*_bin_inc;
            print("percentile_cuttoff: percentile= %n, cutoff= %f \n", percentile, cutoff);
            break;
        }
    }
*/
    free(bin_array);
    return cutoff;
}

void noise_viz_setup_bin(float* arr, int n) {
    percentile_cutoff_calculation(0.80, arr, n);

    printf("===\n");
    printf("Starting Binning\n");
    int i;
    for(i=0; i<num_bins;i++) bin2[i] = 0;
    double average = 0; 
    for(i=0; i<n; i++) {
        average += arr[i];
        //printf("%f \n", arr[n]);
        bin_float(arr[i]);
    }
    //average = average / ((double)(n));

    printf("number of samples= %i\n", n);
    printf("average= %f, sum= %f \n", average/n, average);
    printf("less_than_min= %i \n",less_than_min);
    printf("greater_than_max= %i \n", greater_than_max);
    printf("finished binning\n");
    printf("===\n");
    bin_scale = ((float)(n))/num_bins;
}


void rmf_dist(int x, int y, int z) {
    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    float fz = (float)z + 2.0f;

    float *_noise = (float*) malloc(sizeof(float)*x*y*z);
    
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                _noise[i + x*j + x*y*k] = rmf_perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x, y, z, 0);
            }
        }
    }

    noise_viz_setup_bin(_noise, x*y*z);
    
    free(_noise);
}

void perlin3_dist(int x, int y, int z) {
    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    float fz = (float)z + 2.0f;

    float *_noise = (float*) malloc(sizeof(float)*x*y*z);
    
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                _noise[i + x*j + x*y*k] = perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x, y, z, 0);
            }
        }
    }

    noise_viz_setup_bin(_noise, x*y*z);
    
    free(_noise);
}


#define _C 0.5
#ifdef DC_CLIENT
void draw_noise_viz_histrogram(float x, float y, float z) {
    //printf("po=%i\n", PO_L);
    x -= 10;
    y += 10;
    int i,j;

    //printf("draw\n");
    int _num;
    glBegin(GL_POINTS);
    glColor3ub((unsigned char) 0,(unsigned char)0,(unsigned char)255);
    for(i=0; i<num_bins; i++) {
        //printf("bin[%i]=%i\n", i,bin[i]);
        _num = (bin2[i] / bin_scale)*100.0;
        for(j=0; j<_num; j++) {
            glVertex3f(_C+x+2*j,_C+y+2*i,z);
        }
    }

    glColor3ub((unsigned char) 200,(unsigned char)0,(unsigned char)0);
    for(i=0; i<20; i++) {
        glVertex3f(_C+x-2,_C+y+2*10*i,z);
        glVertex3f(_C+x-3,_C+y+2*10*i,z);
        glVertex3f(_C+x-4,_C+y+2*10*i,z);
    }
    glEnd();

    //printf("time= %i\n", get_current_netpeer_time());
}
#endif
