#pragma once

#if DC_CLIENT
# error Don't include this file in the client
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
Vec3 dig_worm(Vec3 pos /* posart */, float theta, float phi, float cave_size, CubeType ct) {
    while (mrandf() < 0.999f)
    {
        const static float length = 2.0f;
        const float _theta = theta*2*PI;
        const float _phi = phi*2*PI;

        float dx = (sin(_phi) * cos(_theta));
        float dy = (sin(_phi) * sin(_theta));
        //dz += 0.1f; //////////////-------------
        float dz = cosf(0.95f*_phi);
        dz *= dz;

        float size = cave_size * (mrandf() * 1.25f + 0.5f);  // variable diameter

        float xm = abs(dx) + size;
        float ym = abs(dy) + size;
        float zm = abs(dz) + size;

        int xmin = pos.x - xm;
        int xmax = pos.x + xm;
        int ymin = pos.y - ym;
        int ymax = pos.y + ym;
        int zmin = pos.z - zm;
        int zmax = pos.z + zm;


        bool out_of_bounds = false;
        //can speed up by 8
        for (int i=xmin; i<=xmax; i++)
        for (int j=ymin; j<=ymax; j++)
        for (int k=zmin; k<=zmax; k++) {
            if (k < 0 || k >= map_dim.z)  {
                out_of_bounds = true;
                continue;
            }

            float x = ((float)i) + 0.5f;
            float y = ((float)j) + 0.5f;
            float z = ((float)k) + 0.5f;

            int ii = i % map_dim.x;
            int jj = j % map_dim.y;

            float d = cc_point_line_distance2(pos.x,pos.y,pos.z, dx,dy,dz, x,y,z);
            if (d < size*size) t_map::set(ii, jj, k, ct);
        }

        if (out_of_bounds) phi *= -1;

        pos.x += length*dx;
        pos.y += length*dy;
        pos.z += length*dz;

        static const float theta_adj = 0.15f;
        static const float phi_adj = 0.10f;

        theta += theta_adj * ((float)(2.0*mrandf() - 1.0));
        phi   += phi_adj   * ((float)(2.0*mrandf() - 1.0)) / 18;

        //if (phi < 0)  phi += 1; // this prevents changes that are more than %33 percent or so
        //if (phi >= 1) phi -= 1;
    }

    return pos;
}


void excavate() {
    printf("Excavating\n");

    const float cave_size = 2.0f;
    const int try_limit = 10000;
    CubeType worm_brush = EMPTY_CUBE; //t_map::get_cube_type("rock"); /**/
    const int MAX_WORMS = 300;
    Vec3* worms = new Vec3[MAX_WORMS];
    IF_ASSERT(worms == NULL) return;
    worms[0].x = randrange(0, map_dim.x - 1);
    worms[0].y = randrange(0, map_dim.y - 1);
    worms[0].z = randrange(26, map_dim.z - 1);  // 6 considers bedrock
    int curr_num_worms = 1;

    seed_twister(rand());

    for (int i=0; i<MAX_WORMS; i++)
    {
        Vec3 st;  // start point
        int tries = 0;

        do
        {
            st.x = float(mrandf()) * float(map_dim.x);
            st.y = float(mrandf()) * float(map_dim.y);
            st.z = float(mrandf()) * float(map_dim.z);
        } while (t_map::get(st.x, st.y, st.z) == worm_brush && tries++ < try_limit);

        if (tries >= try_limit) return;

        //const float MIN_ASCENT = PI / 2;
        //const float MAX_ASCENT = MIN_ASCENT - PI / 16;
        float phi   = PI; // 0 goes straight up
        float theta = float(mrandf() * 2*PI); // is any angle

        if (curr_num_worms < MAX_WORMS)
        {
            // make a new worm
            Vec3 the_last;
            if (randrange(1, 3) == 1)
                the_last = dig_worm(st,       theta, phi, cave_size, worm_brush);  // random start point
            else
                the_last = dig_worm(worms[i], theta, phi, cave_size, worm_brush);  // start from end of a previously dug worm


            // setup 3 worms to branch off of its endpoint
            worms[curr_num_worms++] = the_last;
            if (curr_num_worms < MAX_WORMS) worms[curr_num_worms++] = the_last;
            if (curr_num_worms < MAX_WORMS) worms[curr_num_worms++] = the_last;
        }
        else
            dig_worm(worms[i], theta, phi, cave_size, worm_brush);
    }

    delete[] worms;
}


}   // t_gen
