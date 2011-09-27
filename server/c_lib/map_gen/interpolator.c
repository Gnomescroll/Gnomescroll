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

    int nx = (x / x_interval) + 4,
        ny = (y / y_interval) + 4,
        nz = (z / z_interval) + 4;

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
        sx = (i / x_interval) + 2; // sample point index in anchor pts array
        
        for (j=0; j<y; j++) {
            my = j % y_interval;
            py = my / fy_interval;
            sy = (j / y_interval) + 2;

            for (k=0; k<z; k++) {
                mz = k % z_interval;
                pz = mz / fz_interval;
                sz = (k / z_interval) + 2;

                // collect local samples
                for (ii=-1; ii < 3; ii++) {
                    for (jj=-1; jj < 3; jj++) {
                        for (kk=-1; kk < 3; kk++) {
                            samples[ii+1][jj+1][kk+1] = points[ii+sx][jj+sy][kk+sz];  // correct this. increased interpolation margins from 2 to 4, so adjust proper code elsewhere
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
    int iix,jjy,kkz;
    for (i=2; i < nx-2; i++) {
        ix = (i-2) * x_interval;

        for (j=2; j < ny-2; j++) {
            iy = (j-2) * y_interval;
            
            for (k=2; k < nz-2; k++) {
                iz = (k-2) * z_interval;
                

                //tx = (i==1 || i == nx-2);
                //ty = (j==1 || j == ny-2);
                //tz = (k==1 || k == nz-2);

                //if (tx && ty && tz) {    // corner
                    //continue;
                //}

                //if (tx && ty) { // edge z
                    //for (kk=0; kk < 2; kk++) {
                        //kkz = (kk == 0) ? 0 : kk;
                        //index = x*y*(iz + kkz);
                        //resamples1[kk] = final[index];
                    //}
                    //pt = linearInterpolate(resamples1, 0.5f);

                //} else if (tx && tz) {  // edge y
                    //for (jj=0; jj < 2; jj++) {
                        //jjy = (jj == 0) ? 0 : jj;
                        //index = x*(iy + jjy);
                        //resamples1[jj] = final[index];
                    //}
                    //pt = linearInterpolate(resamples1, 0.5f);
                    
                //} else if (ty && tz) {  // edge x
                    //for (ii=0; ii < 2; ii++) {
                        //iix = (ii == 0) ? 0 : ii;
                        //index = ix + iix;
                        //resamples1[ii] = final[index];
                    //}
                    //pt = linearInterpolate(resamples1, 0.5f);

                //} else if (tx) {    // side yz
                    //for (jj=0; jj < 2; jj++) {
                        //jjy = (jj == 0) ? 0 : jj;
                        //for (kk=0; kk < 2; kk++) {
                            //kkz = (kk == 0) ? 0 : kk;
                            //index = x*(iy + jjy) + x*y*(iz + kkz);
                            //resamples2[jj][kk] = final[index];
                        //}
                    //}
                    //pt = bilinearInterpolate(resamples2, 0.5f, 0.5f);
                    
                //} else if (ty) {    // size xz

                    //for (ii=0; ii < 2; ii++) {
                        //iix = (ii == 0) ? 0 : ii;
                        //for (kk=0; kk < 2; kk++) {
                            //kkz = (kk == 0) ? 0 : kk;
                            //index = ix + iix + x*y*(iz + kkz);
                            //resamples2[ii][kk] = final[index];
                        //}
                    //}
                    //pt = bilinearInterpolate(resamples2, 0.5f, 0.5f);
                    
                //} else if (tz) {    // side xy

                    //for (ii=0; ii < 2; ii++) {
                        //iix = (ii == 0) ? 0 : ii;
                        //for (jj=0; jj < 2; jj++) {
                            //jjy = (jj == 0) ? 0 : jj;
                            //index = ix + iix + x*(iy + jjy);
                            //resamples2[ii][jj] = final[index];
                        //}
                    //}
                    //pt = bilinearInterpolate(resamples2, 0.5f, 0.5f);
                    
                //} else {

                    for (ii=0; ii < 2; ii++) {
                        iix = (ii == 0) ? -1 : ii;
                        for (jj=0; jj < 2; jj++) {
                            jjy = (jj == 0) ? -1 : jj;
                            for (kk=0; kk < 2; kk++) {
                                kkz = (kk == 0) ? -1 : kk;
                                index = ix + iix + x*(iy + jjy) + x*y*(iz + kkz);
                                resamples3[ii][jj][kk] = final[index];
                                //printf("%f %d\n", final[index], index);
                            }
                        }
                    }

                    // update all the samplers to use the new iteration (above)
                    // just make it a function

                    pt = trilinearInterpolate(resamples3, 0.5f, 0.5f, 0.5f);
                //}

                //if (pt > 0.0f) {printf("%f\n", pt);}
                //printf("%f\n", pt);
                points[i][j][k] = pt;
            }
        }
    }
    
    // merge sample points with interpolated
    for (i=2; i < nx-2; i++) {
        ii = (i-2) * x_interval;

        for (j=2; j < ny-2; j++) {
            jj = (j-2) * y_interval;

            for (k=2; k < nz-2; k++) {
                kk = (k-2) * z_interval;

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

