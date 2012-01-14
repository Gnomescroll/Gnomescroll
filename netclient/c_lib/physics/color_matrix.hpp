#pragma once

/*
    unsigned long *lbuf;
    int xsize, ysize;
    float mat[4][4];

    identmat(mat);

    offsetmat(mat,-50.0,-50.0,-50.0);   // offset color
    cscalemat(mat,1.4,1.5,1.6);     // scale the colors
    saturatemat(mat,1.7);       // saturate by 2.0
    huerotatemat(mat,10.0);     // rotate the hue 10
    printmat(mat);
    applymatrix(lbuf,mat,xsize*ysize);
}
*/

void printmat( float mat[4][4] );

/*
 *  applymatrix -
 *      use a matrix to transform colors.
 */
void applymatrix(unsigned int *ptr, float mat[4][4], int n);

/*
 *  matrixmult -
 *      multiply two matricies
 */
void matrixmult( float a[4][4], float b[4][4], float c[4][4] );

/*
 *  identmat -
 *      make an identity matrix
 */
void identmat( float matrix2[4][4] );

/*
 *  xformpnt -
 *      transform a 3D point using a matrix
 */
void xformpnt(float matrix[4][4],
float x, float y, float z,
float *tx,float *ty,float *tz );

/*
 *  cscalemat -
 *      make a color scale marix
 */
void cscalemat( float mat[4][4], float rscale, float gscale, float bscale );

/*
 *  lummat -
 *      make a luminance marix
 */
void lummat( float mat[4][4] );

/*
 *  saturatemat -
 *      make a saturation marix
 */
void saturatemat(
float mat[4][4],
float sat
);

/*
 *  offsetmat -
 *      offset r, g, and b
 */
void offsetmat(float mat[4][4],
float roffset, float goffset, float boffset);

/*
 *  xrotate -
 *      rotate about the x (red) axis
 */
void xrotatemat( float mat[4][4],
float rs, float rc );

/*
 *  yrotate -
 *      rotate about the y (green) axis
 */
void yrotatemat( float mat[4][4],
float rs, float rc );

/*
 *  zrotate -
 *      rotate about the z (blue) axis
 */
void zrotatemat( float mat[4][4], float rs, float rc );

/*
 *  zshear -
 *      shear z using x and y.
 */
void zshearmat( float mat[4][4], float dx, float dy );

/*
 *  simplehuerotatemat -
 *      simple hue rotation. This changes luminance
 */
void simplehuerotatemat( float mat[4][4], float rot );

/*
 *  huerotatemat -
 *      rotate the hue, while maintaining luminance.
 */
void huerotatemat( float mat[4][4], float rot );

