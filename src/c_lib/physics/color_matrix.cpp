/*
 *  matrix -
 *      Use 4x4 matricies to process color images.
 */
#include "math.h"
#include "stdio.h"

#define RLUM    (0.3086)
#define GLUM    (0.6094)
#define BLUM    (0.0820)

#define OFFSET_R        3
#define OFFSET_G        2
#define OFFSET_B        1
#define OFFSET_A        0

/*
 *  printmat -
 *      print a 4 by 4 matrix
 */


void printmat(float mat[4][4])
{
    int x, y;

    fprintf(stderr,"\n");
    for (y=0; y<4; y++) {
    for (x=0; x<4; x++)
       fprintf(stderr,"%f ",mat[y][x]);
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
}


/*
 *  applymatrix -
 *      use a matrix to transform colors.
 */
void applymatrix(unsigned int *ptr, float mat[4][4], int n)
{
    int ir, ig, ib, r, g, b;
    unsigned char *cptr;

    cptr = (unsigned char *)ptr;
    while (n--) {
    ir = cptr[OFFSET_R];
    ig = cptr[OFFSET_G];
    ib = cptr[OFFSET_B];
    r = ir*mat[0][0] + ig*mat[1][0] + ib*mat[2][0] + mat[3][0];
    g = ir*mat[0][1] + ig*mat[1][1] + ib*mat[2][1] + mat[3][1];
    b = ir*mat[0][2] + ig*mat[1][2] + ib*mat[2][2] + mat[3][2];
    if (r<0) r = 0;
    if (r>255) r = 255;
    if (g<0) g = 0;
    if (g>255) g = 255;
    if (b<0) b = 0;
    if (b>255) b = 255;
    cptr[OFFSET_R] = r;
    cptr[OFFSET_G] = g;
    cptr[OFFSET_B] = b;
    cptr += 4;
    }
}

/*
 *  matrixmult -
 *      multiply two matricies
 */
void matrixmult(float a[4][4], float b[4][4], float c[4][4])
{
    int x, y;
    float temp[4][4];

    for (y=0; y<4 ; y++)
        for (x=0 ; x<4 ; x++) {
            temp[y][x] = b[y][0] * a[0][x]
                       + b[y][1] * a[1][x]
                       + b[y][2] * a[2][x]
                       + b[y][3] * a[3][x];
        }
    for (y=0; y<4; y++)
        for (x=0; x<4; x++)
            c[y][x] = temp[y][x];
}

/*
 *  identmat -
 *      make an identity matrix
 */
void identmat(float matrix2[4][4])
{
    float* matrix = (float *)matrix2;

    *matrix++ = 1.0f;    /* row 1        */
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;    /* row 2        */
    *matrix++ = 1.0f;
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;    /* row 3        */
    *matrix++ = 0.0f;
    *matrix++ = 1.0f;
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;    /* row 4        */
    *matrix++ = 0.0f;
    *matrix++ = 0.0f;
    *matrix++ = 1.0f;
}

/*
 *  xformpnt -
 *      transform a 3D point using a matrix
 */
void xformpnt(float matrix[4][4],
float x, float y, float z,
float *tx,float *ty,float *tz)
{
    *tx = x*matrix[0][0] + y*matrix[1][0] + z*matrix[2][0] + matrix[3][0];
    *ty = x*matrix[0][1] + y*matrix[1][1] + z*matrix[2][1] + matrix[3][1];
    *tz = x*matrix[0][2] + y*matrix[1][2] + z*matrix[2][2] + matrix[3][2];
}

/*
 *  cscalemat -
 *      make a color scale marix
 */
void cscalemat(float mat[4][4], float rscale, float gscale, float bscale)
{
    float mmat[4][4];

    mmat[0][0] = rscale;
    mmat[0][1] = 0.0f;
    mmat[0][2] = 0.0f;
    mmat[0][3] = 0.0f;

    mmat[1][0] = 0.0f;
    mmat[1][1] = gscale;
    mmat[1][2] = 0.0f;
    mmat[1][3] = 0.0f;


    mmat[2][0] = 0.0f;
    mmat[2][1] = 0.0f;
    mmat[2][2] = bscale;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  lummat -
 *      make a luminance marix
 */
void lummat(float mat[4][4])
{
    float mmat[4][4];
    float rwgt, gwgt, bwgt;

    rwgt = (float)RLUM;
    gwgt = (float)GLUM;
    bwgt = (float)BLUM;
    mmat[0][0] = rwgt;
    mmat[0][1] = rwgt;
    mmat[0][2] = rwgt;
    mmat[0][3] = 0.0f;

    mmat[1][0] = gwgt;
    mmat[1][1] = gwgt;
    mmat[1][2] = gwgt;
    mmat[1][3] = 0.0f;

    mmat[2][0] = bwgt;
    mmat[2][1] = bwgt;
    mmat[2][2] = bwgt;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  saturatemat -
 *      make a saturation marix
 */
void saturatemat(
float mat[4][4],
float sat
)

{
    float mmat[4][4];
    float a, b, c, d, e, f, g, h, i;
    float rwgt, gwgt, bwgt;

    rwgt = (float)RLUM;
    gwgt = (float)GLUM;
    bwgt = (float)BLUM;

    a = (1.0f-sat)*rwgt + sat;
    b = (1.0f-sat)*rwgt;
    c = (1.0f-sat)*rwgt;
    d = (1.0f-sat)*gwgt;
    e = (1.0f-sat)*gwgt + sat;
    f = (1.0f-sat)*gwgt;
    g = (1.0f-sat)*bwgt;
    h = (1.0f-sat)*bwgt;
    i = (1.0f-sat)*bwgt + sat;
    mmat[0][0] = a;
    mmat[0][1] = b;
    mmat[0][2] = c;
    mmat[0][3] = 0.0f;

    mmat[1][0] = d;
    mmat[1][1] = e;
    mmat[1][2] = f;
    mmat[1][3] = 0.0f;

    mmat[2][0] = g;
    mmat[2][1] = h;
    mmat[2][2] = i;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  offsetmat -
 *      offset r, g, and b
 */
void offsetmat(float mat[4][4],
float roffset, float goffset, float boffset)

{
    float mmat[4][4];

    mmat[0][0] = 1.0f;
    mmat[0][1] = 0.0f;
    mmat[0][2] = 0.0f;
    mmat[0][3] = 0.0f;

    mmat[1][0] = 0.0f;
    mmat[1][1] = 1.0f;
    mmat[1][2] = 0.0f;
    mmat[1][3] = 0.0f;

    mmat[2][0] = 0.0f;
    mmat[2][1] = 0.0f;
    mmat[2][2] = 1.0f;
    mmat[2][3] = 0.0f;

    mmat[3][0] = roffset;
    mmat[3][1] = goffset;
    mmat[3][2] = boffset;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  xrotate -
 *      rotate about the x (red) axis
 */
void xrotatemat(float mat[4][4],
float rs, float rc)
{
    float mmat[4][4];

    mmat[0][0] = 1.0f;
    mmat[0][1] = 0.0f;
    mmat[0][2] = 0.0f;
    mmat[0][3] = 0.0f;

    mmat[1][0] = 0.0f;
    mmat[1][1] = rc;
    mmat[1][2] = rs;
    mmat[1][3] = 0.0f;

    mmat[2][0] = 0.0f;
    mmat[2][1] = -rs;
    mmat[2][2] = rc;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  yrotate -
 *      rotate about the y (green) axis
 */
void yrotatemat(float mat[4][4],
float rs, float rc)
{
    float mmat[4][4];

    mmat[0][0] = rc;
    mmat[0][1] = 0.0f;
    mmat[0][2] = -rs;
    mmat[0][3] = 0.0f;

    mmat[1][0] = 0.0f;
    mmat[1][1] = 1.0f;
    mmat[1][2] = 0.0f;
    mmat[1][3] = 0.0f;

    mmat[2][0] = rs;
    mmat[2][1] = 0.0f;
    mmat[2][2] = rc;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  zrotate -
 *      rotate about the z (blue) axis
 */
void zrotatemat(float mat[4][4],
float rs, float rc)

{
    float mmat[4][4];

    mmat[0][0] = rc;
    mmat[0][1] = rs;
    mmat[0][2] = 0.0f;
    mmat[0][3] = 0.0f;

    mmat[1][0] = -rs;
    mmat[1][1] = rc;
    mmat[1][2] = 0.0f;
    mmat[1][3] = 0.0f;

    mmat[2][0] = 0.0f;
    mmat[2][1] = 0.0f;
    mmat[2][2] = 1.0f;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  zshear -
 *      shear z using x and y.
 */
void zshearmat(float mat[4][4],
float dx, float dy)
{
    float mmat[4][4];

    mmat[0][0] = 1.0f;
    mmat[0][1] = 0.0f;
    mmat[0][2] = dx;
    mmat[0][3] = 0.0f;

    mmat[1][0] = 0.0f;
    mmat[1][1] = 1.0f;
    mmat[1][2] = dy;
    mmat[1][3] = 0.0f;

    mmat[2][0] = 0.0f;
    mmat[2][1] = 0.0f;
    mmat[2][2] = 1.0f;
    mmat[2][3] = 0.0f;

    mmat[3][0] = 0.0f;
    mmat[3][1] = 0.0f;
    mmat[3][2] = 0.0f;
    mmat[3][3] = 1.0f;
    matrixmult(mmat,mat,mat);
}

/*
 *  simplehuerotatemat -
 *      simple hue rotation. This changes luminance
 */
void simplehuerotatemat(float mat[4][4],
float rot)
{
    float mag;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;

/* rotate the grey vector into positive Z */
    mag = sqrtf(2.0f);
    xrs = 1.0f/mag;
    xrc = 1.0f/mag;
    xrotatemat(mat,xrs,xrc);

    mag = sqrtf(3.0f);
    yrs = -1.0f/mag;
    yrc = sqrtf(2.0f)/mag;
    yrotatemat(mat,yrs,yrc);

/* rotate the hue */
    zrs = (float)sin(rot*M_PI/180.0f);
    zrc = (float)cos(rot*M_PI/180.0f);
    zrotatemat(mat,zrs,zrc);

/* rotate the grey vector back into place */
    yrotatemat(mat,-yrs,yrc);
    xrotatemat(mat,-xrs,xrc);
}

/*
 *  huerotatemat -
 *      rotate the hue, while maintaining luminance.
 */
void huerotatemat(float mat[4][4],
float rot)
{
    float mmat[4][4];
    float mag;
    float lx, ly, lz;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    float zsx, zsy;

    identmat(mmat);

/* rotate the grey vector into positive Z */
    mag = sqrtf(2.0f);
    xrs = 1.0f/mag;
    xrc = 1.0f/mag;
    xrotatemat(mmat,xrs,xrc);
    mag = sqrtf(3.0f);
    yrs = -1.0f/mag;
    yrc = sqrtf(2.0f)/mag;
    yrotatemat(mmat,yrs,yrc);

/* shear the space to make the luminance plane horizontal */
    xformpnt(mmat,(float)RLUM,(float)GLUM,(float)BLUM,&lx,&ly,&lz);
    zsx = lx/lz;
    zsy = ly/lz;
    zshearmat(mmat,zsx,zsy);

/* rotate the hue */
    zrs = (float)sin(rot*2*M_PI);
    zrc = (float)cos(rot*2*M_PI);
    zrotatemat(mmat,zrs,zrc);

/* unshear the space to put the luminance plane back */
    zshearmat(mmat,-zsx,-zsy);

/* rotate the grey vector back into place */
    yrotatemat(mmat,-yrs,yrc);
    xrotatemat(mmat,-xrs,xrc);

    matrixmult(mmat,mat,mat);
}

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
