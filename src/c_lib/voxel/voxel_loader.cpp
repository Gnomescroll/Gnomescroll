#include "voxel_loader.hpp"

#include <common/files.hpp>

namespace Voxels
{

//this function will always return on a new line or null
static void check_for_comments(const char* s, size_t* index)
{
    while (1)
    {
        while (s[*index] == ' ' || s[*index] == '\t' || s[*index] == '\n')
            (*index)++;
        if (s[*index] != '#' || s[*index] == '\0')
            break;
        while (s[*index] != '\n' && s[*index] != '\0')
            (*index)++;
    }
}

void read_skeleton(const char* file_name, VoxDat* vox_dat)
{
    //printf("Loading skeleton: %s \n", file_name);
    size_t size = 0;
    char* buffer = read_file_to_buffer(file_name, &size);
    IF_ASSERT(buffer == NULL) return;

    char* str_tmp = new char[512];
    int n_parts;
    int num_skeleton_nodes;
    size_t index = 0;
    int read;

    //read in number of voxel volumes
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %n", &num_skeleton_nodes, &n_parts, &read);
    index += read;

    vox_dat->init_parts(n_parts);
    vox_dat->init_skeleton(num_skeleton_nodes);


    // skeleton graph
    for (int i=0; i<num_skeleton_nodes; i++)
    {
        int node;
        int parent_skeleton_node;
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %d %n", &node, &parent_skeleton_node, &read);
        index += read;
        vox_dat->set_skeleton_node_parent(node, parent_skeleton_node);
    }

    // skeleton node matrixs
    check_for_comments(buffer, &index);
    for (int i=0; i<num_skeleton_nodes; i++)
    {
        int node;
        float x,y,z;
        float rx,ry,rz;

        sscanf (buffer+index, "%d %f %f %f  %f %f %f %n", &node, &x,&y,&z, &rx,&ry,&rz, &read);
        index += read;
        vox_dat->set_skeleton_local_matrix(node, x,y,z, rx,ry,rz);
    }

    // voxel part size, dimension, color
    for (int i=0; i<n_parts; i++)
    {
        int part_num;
        int skeleton_parent_matrix; // not used?
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %d %s %n", &part_num, &skeleton_parent_matrix, str_tmp, &read);
        index += read;
        read_voxel_volume(str_tmp, part_num, vox_dat);
        vox_dat->set_skeleton_parent_matrix(part_num, skeleton_parent_matrix);
    }

    // voxel part rotation, anchor
    for (int i=0; i<n_parts; i++)
    {
        int part_num;
        float ox,oy,oz;
        float rx,ry,rz;
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d  %f %f %f  %f %f %f %n", &part_num, &ox, &oy, &oz, &rx, &ry, &rz, &read);
        index += read;
        vox_dat->set_part_local_matrix(part_num, ox,oy,oz, rx,ry,rz);
    }

    delete[] str_tmp;
    free(buffer);
}

void read_voxel_volume(const char* file_name, int part_num, VoxDat* vox_dat)
{
    //printf("Loading voxel model: %s \n", file_name);
    size_t size = 0;
    char* buffer = read_file_to_buffer(file_name, &size);
    IF_ASSERT(buffer == NULL) return;

    size_t index = 0;
    int read;

    // dimension
    int xdim, ydim, zdim;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %d %n", &xdim, &ydim, &zdim, &read);
    index += read;

    //voxel size
    float vox_size;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%f %n", &vox_size, &read);
    index += read;

    // biaxial
    int biaxial;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %n", &biaxial, &read);
    index += read;

    vox_dat->set_part_properties(part_num, vox_size, vec3i_init(xdim, ydim, zdim),
                                 file_name, (bool)biaxial);

    // colorable
    int colorable;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %n", &colorable, &read);
    index += read;

    // colorable base color
    int cr,cg,cb;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %d %n", &cr, &cg, &cb, &read);
    index += read;

    vox_dat->set_colorable(part_num, colorable);
    vox_dat->set_base_color(part_num, cr,cg,cb);


    int ret;
    int vox_num = 0;
    int x, y, z, r, g, b;

    while (1)
    {
        check_for_comments(buffer, &index);
        if (buffer[index] == '\0' || index == size)
            break;

        if (index > size)
            printf("voxel_volume_read: buffer overflow, index= %lu, size= %lu \n", (unsigned long)index, (unsigned long)size);

        ret = sscanf(buffer+index, "%d %d %d  %d %d %d  %n", &x,&y,&z,&r,&g,&b, &read);

        if (ret == 0)
        {
            printf("read_voxel_volume: error ret==0 \n");
            break;
        }
        index += read;

        vox_dat->set_color(part_num, vec3i_init(x, y, z), r,g,b, 255);
        vox_num++;
    }

    free(buffer);
}

}   // Voxels
