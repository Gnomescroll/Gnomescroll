from libcpp cimport bool

cdef extern from "./voxel/voxel_body.hpp":
    cdef cppclass VoxBody:
        float vox_size
        void init(int n_parts)
        void set_part(
            float rot_x, float rot_y, float rot_z,
            float rot_ax, float rot_ay, float rot_az,
            float anc_len, float anc_x, float anc_y, float anc_z,
            int dim_x, int dim_y, int dim_z,
            int part_num,
            bool biaxial
        )
        void set_color(int part, int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)


cdef extern from "./items/flag.hpp":
    int FLAG_PART_NUM
    VoxBody flag_vox_dat

    cdef enum FLAG_PARTS:
        FLAG_PART_CLOTH

cdef extern from "./items/base.hpp":
    int BASE_PART_NUM
    VoxBody base_vox_dat

    cdef enum BASE_PARTS:
        BASE_PART_MAIN

flag_cloth=\
{"voxels":[[0,0,0,223,31,31,255],[0,0,1,223,31,31,255],[0,0,2,223,31,31,255],[0,0,3,223,31,31,255],[1,0,0,223,31,31,255],[2,0,0,223,31,31,255],[3,0,0,223,31,31,255],[4,0,0,223,31,31,255],[5,0,0,223,31,31,255],[6,0,0,223,31,31,255],[7,0,0,223,31,31,255],[8,0,0,223,31,31,255],[9,0,0,223,31,31,255],[9,0,1,223,31,31,255],[9,0,2,223,31,31,255],[9,0,3,223,31,31,255],[9,0,4,223,31,31,255],[9,0,5,223,31,31,255],[0,0,4,223,31,31,255],[0,0,5,223,31,31,255],[1,0,5,223,31,31,255],[2,0,5,223,31,31,255],[3,0,5,223,31,31,255],[4,0,5,223,31,31,255],[5,0,5,223,31,31,255],[6,0,5,223,31,31,255],[7,0,5,223,31,31,255],[8,0,5,223,31,31,255],[1,0,1,48,48,48,255],[2,0,1,48,48,48,255],[2,0,2,48,48,48,255],[3,0,2,48,48,48,255],[3,0,3,48,48,48,255],[4,0,3,48,48,48,255],[4,0,4,48,48,48,255],[8,0,4,48,48,48,255],[7,0,4,48,48,48,255],[7,0,3,48,48,48,255],[6,0,3,48,48,48,255],[6,0,2,48,48,48,255],[5,0,2,48,48,48,255],[5,0,1,48,48,48,255],[4,0,1,239,239,239,255],[3,0,1,239,239,239,255],[4,0,2,239,239,239,255],[5,0,3,239,239,239,255],[5,0,4,239,239,239,255],[6,0,4,239,239,239,255],[7,0,1,239,239,239,255],[6,0,1,239,239,239,255],[7,0,2,239,239,239,255],[8,0,2,239,239,239,255],[8,0,3,239,239,239,255],[3,0,4,239,239,239,255],[2,0,3,239,239,239,255],[2,0,4,239,239,239,255],[1,0,2,239,239,239,255],[1,0,3,239,239,239,255],[8,0,1,48,48,48,255],[1,0,4,48,48,48,255]],"vosize":0.2,"dim":[10,1,6]}

flag_parts = {
    FLAG_PART_CLOTH: flag_cloth
}

def load_flag_voxel_dat():

    flag_vox_dat.init(FLAG_PART_NUM)

    for part in range(FLAG_PART_NUM):
        vox = flag_parts[part]

        flag_vox_dat.vox_size = vox['vosize']

        fx,fy,fz,nx,ny,nz,length,ax,ay,az = [0]*10

        xdim,ydim,zdim = vox['dim']
        biaxial = False
        flag_vox_dat.set_part(fx,fy,fz,nx,ny,nz, length,ax,ay,az, xdim,ydim,zdim, part, biaxial)
        
        for color in vox['voxels']:
            x,y,z, r,g,b,a = color
            flag_vox_dat.set_color(part, x,y,z, r,g,b,a)

base_main=\
{"voxels":[[0,2,0,223,31,175,255],[0,3,0,223,31,175,255],[0,4,0,223,31,175,255],[2,6,0,223,31,175,255],[3,6,0,223,31,175,255],[4,6,0,223,31,175,255],[6,4,0,223,31,175,255],[6,2,0,223,31,175,255],[6,3,0,223,31,175,255],[4,0,0,223,31,175,255],[3,0,0,223,31,175,255],[2,0,0,223,31,175,255],[3,0,1,223,31,175,255],[6,3,1,223,31,175,255],[3,6,1,223,31,175,255],[0,3,1,223,31,175,255],[2,5,1,223,31,175,255],[3,5,1,223,31,175,255],[4,5,1,223,31,175,255],[5,4,1,223,31,175,255],[5,3,1,223,31,175,255],[5,2,1,223,31,175,255],[5,3,2,223,31,175,255],[3,5,2,223,31,175,255],[1,4,1,223,31,175,255],[1,3,1,223,31,175,255],[1,2,1,223,31,175,255],[1,3,2,223,31,175,255],[2,1,1,223,31,175,255],[3,1,1,223,31,175,255],[4,1,1,223,31,175,255],[3,1,2,223,31,175,255],[2,2,2,223,31,175,255],[3,2,2,223,31,175,255],[3,2,3,223,31,175,255],[4,2,2,223,31,175,255],[4,3,2,223,31,175,255],[4,4,2,223,31,175,255],[4,3,3,223,31,175,255],[3,4,2,223,31,175,255],[2,4,2,223,31,175,255],[2,3,2,223,31,175,255],[2,3,3,223,31,175,255],[3,4,3,223,31,175,255],[3,3,3,223,31,175,255],[3,3,4,223,31,175,255],[1,5,0,223,31,175,255],[5,5,0,223,31,175,255],[5,1,0,223,31,175,255],[1,1,0,223,31,175,255]],"vosize":0.2,"dim":[7,7,5]}

base_parts = {
    BASE_PART_MAIN: base_main,
}

def load_base_voxel_dat():

    base_vox_dat.init(BASE_PART_NUM)

    for part in range(BASE_PART_NUM):
        vox = base_parts[part]

        base_vox_dat.vox_size = vox['vosize']

        fx,fy,fz,nx,ny,nz,length,ax,ay,az = [0]*10

        xdim,ydim,zdim = vox['dim']
        biaxial = False
        base_vox_dat.set_part(fx,fy,fz,nx,ny,nz, length,ax,ay,az, xdim,ydim,zdim, part, biaxial)
        
        for color in vox['voxels']:
            x,y,z, r,g,b,a = color
            base_vox_dat.set_color(part, x,y,z, r,g,b,a)

def load_voxel_dats():
    load_flag_voxel_dat()
    load_base_voxel_dat()
