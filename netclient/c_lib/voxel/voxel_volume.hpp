#pragma once


class Orientation_matrix
{
    
    float v[4][4];
    //x,y,z,offset

    //multiply current by other
    void multiply(Orientation_matrix* m)
    {
        return;
    }

    Orientation_matrix()
    {
        return;
    }

};

unsigned NextPow2( unsigned x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

int pow2_1(int n)
{
    int x = 1;
    int i = 1;
    while(x < n) 
    {
        i++;
        x <<= 1;
    }
    return i;
} 

int pow2_2(int n)
{
    int x = 1;
    int i = 1;
    while(x < n) 
    {
        i++;
        x <<= 1;
    }
    return n;
} 

#include <voxel/common.h>


class Voxel_volume
{
    float v[3][4]; // forward, up, right (x,y,z), offset
    float scale;    //size of voxels
    //bounding sphere
    float center[3];
    float radius;


    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1, index12;



    Voxel_volume(int __xdim, int __ydim, int __zdim)
    {
        xdim = __xdim;
        ydim = __ydim;
        zdim = __zdim;

        _xdim = pow2_2(__xdim);
        _ydim = pow2_2(__ydim);
        _zdim = pow2_2(__zdim);
        
        index1 = pow2_1(__ydim);
        index12 = pow2_1(__ydim) + pow2_1(__zdim);

        printf("__xdim, __ydim, __zdim= %i, %i, %i \n", __xdim, __ydim,__zdim);
        printf("_xdim, _ydim, _zdim= %i, %i, %i \n", xdim, _ydim, _zdim);
        printf("index1, index12= %i, %i \n", index1, index12);

        voxel = new Voxel[_xdim*_ydim*_zdim];
    }

    ~Voxel_volume()
    {
        delete voxel;
    }
};


void voxel_test()
{
    
    Voxel_volume vv(8,8,8);

}