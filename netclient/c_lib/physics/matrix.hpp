#pragma once



/*
optimization: compute matix and return matrix
*/
// possibly wrong
struct Vector euler_rotation(Vector v, float x, float y, float z)
{   
    x *= 2*PI;
    y *= 2*PI;
    z *= 2*PI;

    double cx = cos(x);
    double sx = sin(x);
    double cy = cos(y);
    double sy = sin(y);
    double cz = cos(z);
    double sz = sin(z);
    
    Vector m[3];
    Vector u;

    m[0].x = (cy*cz); 
    m[0].y = (cy*sz);
    m[0].z = (-sy);

    double sxsy = sx*sy;
    double cxsy = cx*sy;
    
    m[1].x = (sxsy*cz-cx*sz);
    m[1].y = (sxsy*sz+cx*cz);
    m[1].z = (sx*cy);

    m[2].x = (cxsy*cz+sx*sz);
    m[2].y = (cxsy*sz-sx*cz);
    m[2].z = (cx*cy);

    u.x = v.x*m[0].x + v.y*m[1].x + v.z*m[2].x, 
    u.y = v.x*m[0].y + v.y*m[1].y + v.z*m[2].y, 
    u.z = v.x*m[0].z + v.y*m[1].z + v.z*m[2].z;

    return u;
}


//Matix reference implementation

/*
void _3D::RotateFill(double *A, double xAngle, double yAngle, double zAngle)
{
   // Fill the rotation matrix, using Euler angles 

   double x[12];
   double y[12];
   double z[12];
   double tempArray[12];
   double cx,cy,cz;
   double sx,sy,sz;

   cx = cos(xAngle);
   cy = cos(yAngle);
   cz = cos(zAngle);

   sx = sin(xAngle);
   sy = sin(yAngle);
   sz = sin(zAngle);

   x[0]=1;     x[1]=0;     x[2] =0;     x[3] =0;
   x[4]=0;     x[5]=cx;    x[6] =-sx;   x[7] =0;
   x[8]=0;     x[9]=sx;    x[10]=cx;    x[11]=0;

   y[0]=cy;    y[1]=0;     y[2] =sy;    y[3] =0;
   y[4]=0;     y[5]=1;     y[6] =0;     y[7] =0;
   y[8]=-sy;   y[9]=0;     y[10]=cy;    y[11]=0;

   z[0]=cz;    z[1]=-sz;   z[2] =0;     z[3] =0;
   z[4]=sz;    z[5]=cz;    z[6] =0;     z[7] =0;
   z[8]=0;     z[9]=0;     z[10]=1;     z[11]=0;

   //Note we are multiplying x*y*z. You can change the order,
   //  but you will get different results. 

   MatrixMultiply(z,y,tempArray);   // multiply 2 matrices
   MatrixMultiply(tempArray,x,A);   // multiply result by 3rd matrix
}
*/

/*
    inline static void rotate_euler(matrix3 &m, float x, float y, float z)
    {
        double cx = cos(x);
        double sx = sin(x);
        double cy = cos(y);
        double sy = sin(y);
        double cz = cos(z);
        double sz = sin(z);
        
        m.col[0].x = (float)(cy*cz); 
        m.col[0].y = (float)(cy*sz);
        m.col[0].z = (float)(-sy);

        double sxsy = sx*sy;
        double cxsy = cx*sy;
        
        m.col[1].x = (float)(sxsy*cz-cx*sz);
        m.col[1].y = (float)(sxsy*sz+cx*cz);
        m.col[1].z = (float)(sx*cy);
    
        m.col[2].x = (float)(cxsy*cz+sx*sz);
        m.col[2].y = (float)(cxsy*sz-sx*cz);
        m.col[2].z = (float)(cx*cy);
    }

*/
