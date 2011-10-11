#include <c_lib/noise_viz.hpp>
//srand((unsigned)time(0));

int _num_bins = 1000; //parameter

float percentile_cutoff_calculation(float percentile, float* arr, int n) {

    int i;
    printf("=== Start percentile_cutoff_calculation ===\n");
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
        bin_index = (arr[i]-min) / _bin_inc;
        bin_array[bin_index]++;
    }
    //do percentile calculation
    int counter = 0;
    int terminal = percentile* ((float)(n));
    float cutoff = -1;
    for(i=0; i<_num_bins; i++) {
        counter += bin_array[i];
        //printf("i=%i, count=%i \n", i, counter);
        if(counter > terminal) {
            cutoff = min+i*_bin_inc;
            printf("debug: count=%i, samples=%i, terminal= %i i=%i of %i, min=%f, _bin_inc=%f\n", counter, n, terminal, i, _num_bins, min, _bin_inc);
            printf("percentile_cuttoff: percentile= %f, cutoff= %f \n", percentile, cutoff);
            break;
        }
    }

    free(bin_array);
    return cutoff;
}

