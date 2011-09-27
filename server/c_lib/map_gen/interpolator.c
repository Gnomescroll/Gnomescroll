#include <c_lib/t_map/t_map.h>

// For lowering sample resolution and interpolating in between


/*
 * Interpolation methods borrowed from
 *
 * http://paulbourke.net/miscellaneous/interpolation/index.html
 *
 */
float cubicInterpolate (float p[4], float x) {
    return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

float bicubicInterpolate (float p[4][4], float x, float y) {
    float arr[4];
    arr[0] = cubicInterpolate(p[0], y);
    arr[1] = cubicInterpolate(p[1], y);
    arr[2] = cubicInterpolate(p[2], y);
    arr[3] = cubicInterpolate(p[3], y);
    return cubicInterpolate(arr, x);
}

float tricubicInterpolate (float p[4][4][4], float x, float y, float z) {
    float arr[4];
    arr[0] = bicubicInterpolate(p[0], y, z);
    arr[1] = bicubicInterpolate(p[1], y, z);
    arr[2] = bicubicInterpolate(p[2], y, z);
    arr[3] = bicubicInterpolate(p[3], y, z);
    return cubicInterpolate(arr, x);
}

void _interp(float final[], int x, int y, int z, int x_interval, int y_interval, int z_interval) {

    int nx = (x / x_interval) + 2,
        ny = (y / y_interval) + 2,
        nz = (z / z_interval) + 2;

    float fnx = (float)nx,
           fny = (float)ny,
           fnz = (float)nz;

    float points[nx][ny][nz];

    // fractional interval between sample points
    float fx_interval = (float)x_interval,
           fy_interval = (float)y_interval,
           fz_interval = (float)z_interval;

    // generate anchor points
    int i,j,k;
    for (i=0; i<nx; i++) {
        for (j=0; j<ny; j++) {
            for (k=0; k<nz; k++) {
                points[i][j][k] = perlin3(i/fnx, j/fny, k/fnz, 6, 0.5, 1.0, 1.0, 1024, 1024, 1024, 0);
            }
        }
    }

    // interpolate
    int mx,my,mz;
    float px,py,pz;

    int sx,sy,sz;
    int ii,jj,kk;
    float samples[4][4][4];
    float pt;
    
    for (i=0; i<x; i++) {
        mx = i % x_interval;
        px = mx / fx_interval;
        sx = (i / x_interval); // sample point 0. will use samples -1, 0, 1, 2
        
        for (j=0; j<y; j++) {
            my = j % y_interval;
            py = my / fy_interval;
            sy = (j / y_interval);

            for (k=0; k<z; k++) {
                mz = k % z_interval;
                pz = mz / fz_interval;
                sz = (k / z_interval);

                // collect local samples
                for (ii=0; ii < 4; ii++) {
                    for (jj=0; jj < 4; jj++) {
                        for (kk=0; kk < 4; kk++) {
                            samples[ii][jj][kk] = points[ii+sx-1][jj+sy-1][kk+sz-1];
                        }
                    }
                }
                
                pt = tricubicInterpolate(samples, px, py, pz);
                final[i + x*j + x*y*k] = pt;
            }
        }
    }

    // merge sample points with interpolated
    for (i=1; i < nx-1; i++) {
        ii = (i-1) * x_interval;

        for (j=1; j < ny-1; j++) {
            jj = (j-1) * y_interval;

            for (k=1; k < nz-1; k++) {
                kk = (k-1) * z_interval;

                final[ii + x*jj + x*y*kk] = points[i][j][k];
            }
        }
    }

    //// recalculate the anchor points from the interpolated points
    //for (i=1; i < nx-1; i++) {
        //ii = (i-1) * x_interval;

        //for (j=1; j < ny-1; j++) {
            //jj = (j-1) * y_interval;

            //for (k=1; k < nz-1; k++) {
                //kk = (k-1) * z_interval;

                //for (ii=0; ii < 4; ii++) {
                    //for (jj=0; jj < 4; jj++) {
                        //for (kk=0; kk < 4; kk++) {
                            //samples[ii][jj][kk] = final[ii + x*jj + x*y*kk];
                        //}
                    //}
                //}
                
                ////final[ii + x*jj + x*y*kk] = points[i][j][k];
                //final[ii + x*jj + x*y*kk] = tricubicInterpolate(samples, points[i][j][k];
            //}
        //}
    //}
        
}


float final_interp[xmax*ymax*zmax];

void interp( int x, int y, int z, int x_interval, int y_interval, int z_interval) {
    if (x > xmax || y > ymax || z > zmax || x < 0 || y < 0 || z < 0) {
        printf("interpolation error: dimensions out of map range\n");
        return;
    }
    
    _interp(final_interp, x,y,z, x_interval, y_interval, z_interval);

    // set terrain
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                if (final_interp[i + x*j + x*y*k] > 0) {
                    _set(i,j,k, 2);
                }
            }
        }
    }
}

