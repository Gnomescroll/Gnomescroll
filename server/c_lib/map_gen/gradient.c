void gradient2d(float f[], int x_size, int y_size, float x0, float y0, float x1, float y1) { // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis

    //float samples[2][2] = { {x_pos, y_pos}, {x_neg, y_neg} };
    //float samples[2][2] = { {x_pos, y_neg}, {x_neg, y_pos} };
    float samples[2][2] = { {(x0+y0)/2, (x0+y1)/2}, {(x1+y0)/2, (x1+y1)/2} };

    int i,j;
    float px,py;
    float fx_size = (float)x_size,
            fy_size = (float)y_size;
    
    for (i=0; i<x_size; i++) {
        px = i / fx_size;
        for (j=0; j<y_size; j++) {
            py = j / fy_size;
            f[i + x_size*j] = bilinearInterpolate(samples, px,py);
        }
    }
}

inline void buildGradientSampleCube(float samples[2][2][2], float x0, float y0, float z0, float x1, float y1, float z1) {
    samples[0][0][0] = (x0 + y0 + z0)/3; // 000
    samples[0][0][1] = (x0 + y0 + z1)/3; // 001
    samples[0][1][0] = (x0 + y1 + z0)/3; // 010
    samples[0][1][1] = (x0 + y1 + z1)/3; // 011
    samples[1][0][0] = (x1 + y0 + z0)/3; // 100
    samples[1][0][1] = (x1 + y0 + z1)/3; // 101
    samples[1][1][0] = (x1 + y1 + z0)/3; // 110
    samples[1][1][1] = (x1 + y1 + z1)/3; // 111
}

    
void gradient3d(float f[], int x_size, int y_size, int z_size, float x0, float y0, float z0, float x1, float y1, float z1) { // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis

    float samples[2][2][2];
    buildGradientSampleCube(samples, x0, y0, z0, x1, y1, z1);

    int i,j,k;
    float px,py,pz;
    float fx_size = (float)x_size,
            fy_size = (float)y_size,
            fz_size = (float)z_size;

    for (i=0; i<x_size; i++) {
        px = i / fx_size;
        for (j=0; j<y_size; j++) {
            py = j / fy_size;
            for (k=0; k<z_size; k++) {
                pz = k / fz_size;
                f[i + x_size*j + x_size*y_size*k] = trilinearInterpolate_condensed(samples, px,py,pz);
            }
        }
    }
}

void apply_grad3d(int x, int y, int z, float x_pos, float y_pos, float z_pos, float x_neg, float y_neg, float z_neg) {

    // generate gradient
    float fgrad[x*y*z];
    gradient3d(fgrad, x,y,z, x_pos, y_pos, z_pos, x_neg, y_neg, z_neg);

    // generate map
    interp(x,y,z, 4,4,2);

    // set map
    int i,j,k;
    int index;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                index = i + x*j + x*y*k;
                if (final_interp[index] + fgrad[index] > 0) {    // apply gradient to map
                    _set(i,j,k, 2);
                }
            }
        }
    }
}
