#include "voxel_loader.hpp"

//#include <c_lib/voxel/voxel_skeleton.hpp>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

off_t fsize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

//this function will always return on a new line or null
void check_for_comments(char* s, int* index)
{   
    while (1)
    {
        while(s[*index] == ' ' || s[*index] == '\t' || s[*index] == '\n')
            (*index)++;
        if(s[*index] != '#' || s[*index] == '\0')
            break;
        while(s[*index] != '\n' && s[*index] != '\0')
            (*index)++;
    }
}

char* read_file_to_buffer(char* file_name, int* size)
{
    int expected_size = fsize(file_name);
    char *source = NULL;
    FILE *fp = fopen(file_name, "r");
    if (fp != NULL)
    {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) 
        {
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            *size = (int)bufsize;
            if(*size != expected_size) printf("Warning: size of %s differs from expected\n", file_name);
            if (bufsize == -1) { /* Error */ }
            /* Allocate our buffer to that size. */
            source = (char*) calloc(bufsize+2, sizeof(char));
            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) == 0) { /* Error */ }
            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) 
            {
                printf("read_file_to_buffer: error reading file %s", file_name);
                return NULL;
            } 
            else 
            {
                source[++newLen] = '\0'; /* Just to be safe. */
            }
        }
        if(fp == NULL) printf("WTF\n");
        fclose(fp);
    }
    //free(source); /* Don't forget to call free() later! */
    return source;
}


void read_skeleton(char* file_name, VoxDat* vox_dat)
{
    //printf("Loading skeleton: %s \n", file_name);
    int size;
    char* buffer = read_file_to_buffer(file_name, &size);
    if(buffer == NULL) printf("error reading %s \n", file_name);

    char* str_tmp = new char[512];
    int n_parts;
    int num_skeleton_nodes;
    int index = 0;
    int read;

    //read in number of voxel volumes
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %n", &num_skeleton_nodes, &n_parts, &read);
    index += read;

    vox_dat->init_parts(n_parts);
    vox_dat->init_skeleton(num_skeleton_nodes);


    // skeleton graph
    for(int i=0; i<num_skeleton_nodes; i++)
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
    for(int i=0; i<num_skeleton_nodes; i++)
    {
        int node;
        float x,y,z;
        float rx,ry,rz;

        sscanf (buffer+index, "%d %f %f %f  %f %f %f %n", &node, &x,&y,&z, &rx,&ry,&rz, &read);
        index += read;
        printf("node=%d\n", node);
        vox_dat->set_skeleton_local_matrix(node, x,y,z, rx,ry,rz);
    }

    // voxel part size, dimension, color
    for(int i=0; i<n_parts; i++)
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

void read_voxel_volume(char* file_name, int part_num, VoxDat* vox_dat)
{
    //printf("Loading voxel model: %s \n", file_name);
    int size = 0;
    char* buffer = read_file_to_buffer(file_name, &size);
    if(buffer == NULL)
    {
        printf("error reading %s \n", file_name);
        return;
    }

    int index = 0;
    int read;

    // dimension
    int xdim,ydim,zdim;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %d %n", &xdim,&ydim,&zdim, &read);
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

    vox_dat->set_part_properties(part_num, vox_size, xdim, ydim, zdim, file_name, (bool)biaxial);

    // team
    int team;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %n", &team, &read);
    index += read;

    // team base color
    int team_r, team_g, team_b;
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %d %n", &team_r, &team_g, &team_b, &read);
    index += read;

    #ifdef DC_CLIENT
    vox_dat->set_team(part_num, (bool)team, (unsigned char)team_r, (unsigned char)team_g, (unsigned char)team_b);
    #endif

    int ret;
    int vox_num = 0;
    int x,y,z,r,g,b;

    while(1)
    {
        check_for_comments(buffer, &index);
        if (buffer[index] == '\0' || index == size)
            break;

        if (index > size)
            printf("voxel_volume_read: buffer overflow, index= %i, size= %i \n", index, size);

        ret = sscanf (buffer+index, "%d %d %d  %d %d %d  %n", &x,&y,&z,&r,&g,&b, &read);

        if (ret == 0)
        {
            printf("read_voxel_volume: error ret==0 \n");
            break;
        }
        index += read;

        #ifdef DC_CLIENT
        vox_dat->set_color(part_num, x,y,z, (unsigned char)r,(unsigned char)g,(unsigned char)b, 255);
        #endif
        vox_num++;
    }

    free(buffer);
}
