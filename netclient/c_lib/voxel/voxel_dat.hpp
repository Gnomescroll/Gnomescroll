#pragma once

//#include <physics/Affine.hpp>
#include <physics/affine.hpp>
/* Dat storage */

class VoxDat; // forward decl;

#ifdef DC_CLIENT
class VoxColors {
    public:
        unsigned char** rgba;
        int **index;
        int n;

        bool team;
        unsigned char team_r,team_g,team_b;
        
        void init(int dx, int dy, int dz);
        void set(int i, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_team_base(unsigned char r, unsigned char g, unsigned char b);
        void set_team(bool team);
        VoxColors();
        ~VoxColors();
};
#endif

class VoxPartDimension {
    public:
        int x,y,z;

        void set(int x, int y, int z);
        int count();

        VoxPartDimension();
        VoxPartDimension(int x, int y, int z);
};

class VoxPart {
    public:
        VoxPartDimension dimension;
        #ifdef DC_CLIENT
        VoxColors colors;
        #endif
        VoxDat* dat;    // parent
        
        int part_num;
        
        int skeleton_parent_matrix;
        float sx, sy, sz, srx, sry, srz;    // skeleton local matrix parameters
        
        float vox_size;
        bool biaxial; // true for horizontal+vertical (head). default=false

        void set_local_matrix();   // uses cached x,y,z,rx,ry,rz values
        void set_dimension(int x, int y, int z);

        VoxPart(
            VoxDat* dat,
            int part_num,
            float vox_size,
            int dimension_x, int dimension_y, int dimension_z,
            bool biaxial=false
        );
};

class VoxDat {
    public:
        //voxel volume
        bool voxel_volume_inited;
        int n_parts;
        class VoxPart** vox_part;

        struct Affine* vox_volume_local_matrix;
        bool voxel_skeleton_inited;
        int* vox_skeleton_transveral_list;
        struct Affine* vox_skeleton_local_matrix;
        int n_skeleton_nodes;
        
        void init_skeleton(int n_skeleton);
        
        void set_skeleton_local_matrix(int node, float x, float y, float z, float rx, float ry, float rz);
        void set_skeleton_node_parent(int node, int parent);
        void set_skeleton_parent_matrix(int part, int parent);

        void init_parts(int n_parts);

        void set_part_properties(
            int part_num,
            float vox_size,
            int dimension_x, int dimension_y, int dimension_z,
            bool biaxial=false
        );

        //anchor x,y,z then rotation x,y,z
        void set_part_local_matrix( int part_num, float x, float y, float z, float rx, float ry, float rz);

        #ifdef DC_CLIENT
        void set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_team(int part, bool team, unsigned char r, unsigned char g, unsigned char b);
        #endif

        VoxDat();
        ~VoxDat();
};
