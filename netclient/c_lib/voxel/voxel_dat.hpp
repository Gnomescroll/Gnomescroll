#pragma once

#include <physics/mat4.hpp>

/* Dat storage */

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

class VoxPartRotation {
    public:
        float x,y,z;

        void set(float x, float y, float z);

        VoxPartRotation();
        VoxPartRotation(float x, float y, float z);
};

class VoxPartAnchor {
    public:
        float x,y,z;

        void set(float x, float y, float z);

        VoxPartAnchor();
        VoxPartAnchor(float x, float y, float z);
};

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
        VoxPartRotation rotation;
        VoxPartAnchor anchor;
        VoxPartDimension dimension;
        #ifdef DC_CLIENT
        VoxColors colors;
        #endif
        
        int part_num;
        int skeleton_parent_matrix;

        float vox_size;
        bool biaxial; // true for horizontal+vertical (head). default=false

        void set_rotation(float x, float y, float z);
        void set_anchor(float x, float y, float z);
        void set_dimension(int x, int y, int z);

        VoxPart(
            int part_num,
            float vox_size,
            int dimension_x, int dimension_y, int dimension_z,
            bool biaxial=false
        );
        
        VoxPart(
            int part_num,
            float vox_size,
            int dimension_x, int dimension_y, int dimension_z,
            float anchor_x, float anchor_y, float anchor_z,
            float rotation_x, float rotation_y, float rotation_z,
            bool biaxial=false
        );
};

class VoxDat {
    public:


        bool voxel_volume_inited;

        int n_parts;
        class VoxPart** vox_part;
        struct Mat4* vox_volume_local_matrix;

        bool voxel_skeleton_inited;
        int* vox_skeleton_transveral_list;
        struct Mat4* vox_skeleton_local_matrix;
        int n_skeleton_nodes;
        
        void init_skeleton(int n_skeleton);
        
        void set_skeleton_node_matrix(int node, float x, float y, float z, float rx, float ry, float rz);
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
