#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/noise_map2.hpp>
#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>

#include <t_mech/_interface.hpp>
#include <t_mech/properties.hpp>

namespace t_gen 
{

// v is starting point of line
// vz is unit vector for direction of line
// px is the point
// returns square of distance between line and point
OPTIMIZED
inline float cc_point_line_distance2(float vx, float vy, float vz, float wx, float wy, float wz, float px, float py, float pz) {
    px -= vx;
    py -= vy;
    pz -= vz;

    float t = px*wx + py*wy + pz*wz;

    float x = t*wx - px;
    float y = t*wy - py;
    float z = t*wz - pz;

    return x*x + y*y + z*z;
}



OPTIMIZED
void dig_worm(Vec3 st /* start */, float theta, float phi, float cave_size) {
    while( genrand_real1() < 0.999f ) {
        const static float length = 2.0f;
        const float _theta = theta*2*PI;
        const float _phi = phi*2*PI;

        float dx = (float)(sin(_phi)*cos(_theta));
        float dy = (float)(sin(_phi)*sin(_theta));
        //dz += 0.1f; //////////////-------------
        float dz = cosf(0.95f*_phi);
        dz *= dz;

        float size = cave_size * ((float)genrand_real1()*1.25f + 0.5f);  // variable diameter

        float xm = abs(dx) + size;
        float ym = abs(dy) + size;
        float zm = abs(dz) + size;

        int xmin = st.x - xm;   int xmax = st.x + xm;
        int ymin = st.y - ym;   int ymax = st.y + ym;
        int zmin = st.z - zm;   int zmax = st.z + zm;


        bool out_of_bounds = false;
        //can speed up by 8
        for(int i=xmin; i<=xmax; i++)
        for(int j=ymin; j<=ymax; j++)
        for(int k=zmin; k<=zmax; k++) {
            if(k < 0 || k >= ZMAX)  {
                out_of_bounds = true;
                continue;
            }

            float x = ((float)i) + 0.5f;
            float y = ((float)j) + 0.5f;
            float z = ((float)k) + 0.5f;

            int ii = i%XMAX;
            int jj = j%YMAX;

            float d = cc_point_line_distance2(st.x,st.y,st.z, dx,dy,dz, x,y,z);
            if(d < size*size) t_map::set(ii, jj, k, EMPTY_CUBE);
        }

        if(out_of_bounds == true) phi *= -1;

        st.x += length*dx;
        st.y += length*dy;
        st.z += length*dz;

        static const float theta_adj = 0.15f;
        static const float phi_adj = 0.10f;

        theta += theta_adj*((float)(2.0*genrand_real1() - 1.0));
        phi += phi_adj*((float)(2.0*genrand_real1() - 1.0));

        if(phi < 0) phi += 1;
        if(phi > 1) phi -= 1;
    }

}


void excavate() {
    printf("Excavating\n");

    const int nodes = 300;
    const float cave_size = 2.0f;
    const int try_limit = 10000;
    
    init_genrand(rand());

    for (int i=0; i<nodes; i++) {
        Vec3 st; // start point
        int tries = 0;
        do {
            st.x = (float)genrand_real1()*(float)XMAX;
            st.y = (float)genrand_real1()*(float)YMAX;
            st.z = (float)genrand_real1()*(float)ZMAX;
        } while (t_map::get(st.x, st.y, st.z) == EMPTY_CUBE && tries++ < try_limit);

        if (tries >= try_limit) return; 

        float phi = (float)genrand_real1()*2*3.14159f;
        float theta = (float)genrand_real1()*2*3.14159f;
        
        dig_worm(st, theta, phi, cave_size);
    }
}


}   // t_gen
