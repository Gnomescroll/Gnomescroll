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

class VoxPartOrientation {
    public:
        float x,y,z;

        void set(float x, float y, float z);

        VoxPartOrientation();
        VoxPartOrientation(float x, float y, float z);
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
        VoxPartOrientation orientation;
        VoxPartDimension dimension;
        #ifdef DC_CLIENT
        VoxColors colors;
        #endif
        
        int part_num;
        float vox_size;
        bool biaxial; // true for horizontal+vertical (head). default=false

        void set_rotation(float x, float y, float z);
        void set_orientation(float x, float y, float z);
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
            float orientation_x, float orientation_y, float orientation_z,
            float rotation_x, float rotation_y, float rotation_z,
            bool biaxial=false
        );
};

class VoxBody {
    public:
        class VoxPart** vox_part;

        bool inited;
        int n_parts;

        int* vox_skeleton_transveral_list;
        struct Mat4* vox_skeleton_local_matrix;
        int n_skeleton_nodes;
        
        void init_skeleton(int n_skeleton);
        
        void set_skeleton_node_matrix(int node, float x, float y, float z, float rx, float ry, float rz);
        void set_skeleton_node_parent(int node, int parent);

        void init_parts(int n_parts);

        void set_part(
            int part_num,
            float vox_size,
            int dimension_x, int dimension_y, int dimension_z,
            float orientation_x, float orientation_y, float orientation_z,
            float rotation_x, float rotation_y, float rotation_z,
            bool biaxial=false
        );
        
        void set_part_properties(
            int part_num,
            float vox_size,
            int dimension_x, int dimension_y, int dimension_z,
            bool biaxial=false
        );

        void set_part_spatials(
            int part_num,
            float orientation_x, float orientation_y, float orientation_z,
            float rotation_x, float rotation_y, float rotation_z
        );

        #ifdef DC_CLIENT
        void set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_team(int part, bool team, unsigned char r, unsigned char g, unsigned char b);
        #endif

        void where() {
            printf("VoxBody instantiated at: %p\n", this);
        }

        VoxBody();
        ~VoxBody();
};
