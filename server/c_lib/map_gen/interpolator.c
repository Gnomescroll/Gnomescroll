#include <c_lib/t_map/t_map.h>

// For lowering sample resolution and interpolating in between


/*
 * Interpolation methods borrowed from
 *
 * http://www.paulinternet.nl/?page=bicubic

 * see also:
 * http://paulbourke.net/miscellaneous/interpolation/index.html
 *
 */
float cubicInterpolate (float p[4], float x) {
    return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));  /* Catmull-Rom splines */
}

float cubicInterpolate_standard (float p[4], float x) {
    return p[1] + x*(p[2] - p[0] + x*(2.0*(p[0] - p[1]) + p[2] - p[3] + x*(p[1] - p[2] + p[3] - p[0]))); /* cubic (standard) */
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


float linearInterpolate (float p[2], float x) {
    return x * (p[1] - p[0]) + p[0];
}

float bilinearInterpolate (float p[2][2], float x, float y) {
    float arr[2];
    arr[0] = linearInterpolate(p[0], y);
    arr[1] = linearInterpolate(p[1], y);
    return linearInterpolate(arr, x);
}

float trilinearInterpolate (float p[2][2][2], float x, float y, float z) {
    float arr[2];
    arr[0] = bilinearInterpolate(p[0], y, z);
    arr[1] = bilinearInterpolate(p[1], y, z);
    return linearInterpolate(arr, x);
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
                points[i][j][k] = perlin3(i/fnx, j/fny, k/fnz, 6, 0.5f, 1.0f, 1.0f, 1024, 1024, 1024, 0);
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
                            samples[ii][jj][kk] = points[ii+sx][jj+sy][kk+sz];    // bug
                        }
                    }
                }
                
                pt = tricubicInterpolate(samples, px, py, pz);
                final[i + x*j + x*y*k] = pt;
            }
        }
    }

    //// recalculate the anchor points from the interpolated points
    // use linear interpolation

    float resamples3[2][2][2];
    float resamples2[2][2];
    float resamples1[2];
    int ix,iy,iz;
    int index;
    int tx=0,ty=0,tz=0;
    for (i=1; i < nx-1; i++) {
        ix = i * x_interval;

        for (j=1; j < ny-1; j++) {
            iy = j * y_interval;
            
            for (k=1; k < nz-1; k++) {
                iz = k * z_interval;
                

                //tx = (i==1 || i == nx-2);
                //ty = (j==1 || j == ny-2);
                //tz = (k==1 || k == nz-2);
                //tx=ty=tz=0;

                if (tx && ty && tz) {    // corner
                    continue;
                }

                if (tx && ty) { // edge z
                    for (kk=-1; kk < 2; kk+=2) {
                        index = x*y*(iz + kk);
                        kk = (kk < 0) ? 0 : kk;
                        resamples1[kk] = final[index];
                    }
                    pt = linearInterpolate(resamples1, 0.5f);

                } else if (tx && tz) {  // edge y
                    for (jj=-1; jj < 2; jj+=2) {
                        index = x*(iy + jj);
                        jj = (jj < 0) ? 0 : jj;
                        resamples1[jj] = final[index];
                    }
                    pt = linearInterpolate(resamples1, 0.5f);
                    
                } else if (ty && tz) {  // edge x
                    for (ii=-1; ii < 2; ii+=2) {
                        index = ix + ii;
                        ii = (ii < 0) ? 0 : ii;
                        resamples1[ii] = final[index];
                    }
                    pt = linearInterpolate(resamples1, 0.5f);

                } else if (tx) {    // side yz
                    for (jj=-1; jj < 2; jj+=2) {
                        for (kk=-1; kk < 2; kk+=2) {
                            index = x*(iy + jj) + x*y*(iz + kk);
                            jj = (jj < 0) ? 0 : jj;
                            kk = (kk < 0) ? 0 : kk;
                            resamples2[jj][kk] = final[index];
                        }
                    }
                    pt = bilinearInterpolate(resamples2, 0.5f, 0.5f);
                    
                } else if (ty) {    // size xz
                    for (ii=-1; ii < 2; ii+=2) {
                        for (kk=-1; kk < 2; kk+=2) {
                            index = ix + ii + x*y*(iz + kk);
                            ii = (ii < 0) ? 0 : ii;
                            kk = (kk < 0) ? 0 : kk;
                            resamples2[ii][kk] = final[index];
                        }
                    }
                    pt = bilinearInterpolate(resamples2, 0.5f, 0.5f);
                    
                } else if (tz) {    // side xy
                    for (ii=-1; ii < 2; ii+=2) {
                        for (jj=-1; jj < 2; jj+=2) {
                            index = ix + ii + x*(iy + jj);
                            ii = (ii < 0) ? 0 : ii;
                            jj = (jj < 0) ? 0 : jj;
                            resamples2[ii][jj] = final[index];
                        }
                    }
                    pt = bilinearInterpolate(resamples2, 0.5f, 0.5f);
                    
                } else {

                    for (ii=-1; ii < 2; ii+=2) {
                        for (jj=-1; jj < 2; jj+=2) {
                            for (kk=-1; kk < 2; kk+=2) {
                                index = ix + ii + x*(iy + jj) + x*y*(iz + kk);
                                ii = (ii < 0) ? 0 : ii;
                                jj = (jj < 0) ? 0 : jj;
                                kk = (kk < 0) ? 0 : kk;
                                resamples3[ii][jj][kk] = final[index];
                            }
                        }
                    }

                    pt = trilinearInterpolate(resamples3, 0.5f, 0.5f, 0.5f);
                }

                points[i][j][k] = pt;
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

