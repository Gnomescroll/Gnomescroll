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

void gradient3d(float f[], int x_size, int y_size, int z_size, float x0, float y0, float z0, float x1, float y1, float z1) { // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis

    float samples[2][2][2] = {
                            {
                                {
                                    (x0 + y0 + z0)/3, // 000
                                    (x0 + y0 + z1)/3  // 001
                                }, {
                                    (x0 + y1 + z0)/3, // 010
                                    (x0 + y1 + z1)/3  // 011
                                }
                            }, {
                                {
                                    (x1 + y0 + z0)/3, // 100
                                    (x1 + y0 + z1)/3  // 101
                                }, {
                                    (x1 + y1 + z0)/3, // 110
                                    (x1 + y1 + z1)/3  // 111
                                }
                            }
    };

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
                f[i + x_size*j + x_size*y_size*k] = trilinearInterpolate(samples, px,py,pz);
            }
        }
    }
}

void apply_grad3d(int x, int y, int z, float x_pos, float y_pos, float z_pos, float x_neg, float y_neg, float z_neg) {

    // generate gradient
    float fgrad[x*y*z];
    gradient3d(fgrad, x,y,z, x_pos, y_pos, z_pos, x_neg, y_neg, z_neg);
    //gradient2d(fgrad, x,y, x_pos, y_pos, x_neg, y_neg);
    //gradient2d(fgrad, x,y, 0.5f, y_pos, -0.5f, y_neg);

    // generate map
    //float finterp[x*y*z];
    //_interp(finterp, x,y,z, 4,4,2);
    interp(x,y,z, 4,4,2);

    // set map
    int i,j,k;
    int index;
    float grad;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            //grad = fgrad[i + x*j];
            //printf("%f\n", grad);
            for (k=0; k<z; k++) {
                index = i + x*j + x*y*k;
                if (final_interp[index] + fgrad[index] > 0) {    // apply gradient to map
                //if (final_interp[index] + grad > 0) {    // apply gradient to map
                    _set(i,j,k, 2);
                }
            }
        }
    }
}
