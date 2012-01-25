#include "voxel_loader.hpp"

#include <c_lib/voxel/voxel_skeleton.hpp>
#include <sys/stat.h>

off_t fsize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

//this function will always return on a new line or null
void check_for_comments(char* s, int* index)
{   
    jmp:
    while(s[*index] == ' ' || s[*index] == '\t' || s[*index] == '\n') (*index)++;
    if(s[*index] != '#' || s[*index] == '\0') return;
    while(s[*index] != '\n' && s[*index] != '\0') (*index)++;
    goto jmp;
}

void read_skeleton(char* file_name, VoxBody* vox_dat)
{
    int size = fsize(file_name);
    char* buffer = new char[size+1];
    FILE *fp = fopen(file_name, "r"); //open file for reading
    int nbytes = fread(buffer, sizeof(char), size, fp);
    if ( nbytes != size )
    {
        printf("read_skeleton: failed to write %i bytes\n", nbytes);
        fclose(fp);
        delete[] buffer;
        return;       
    }
    buffer[size] = '\0';

    printf("Loading skeleton: %s \n", file_name);

    char* str_tmp = new char[512];
    int n_parts;
    int num_skeleton_nodes;
    int index = 0;
    int read;

    //read in number of voxel volumes
    check_for_comments(buffer, &index);
    sscanf (buffer+index, "%d %d %n", &num_skeleton_nodes, &n_parts, &read);
    index += read;

    vox_dat->init_skeleton(num_skeleton_nodes);
    vox_dat->init_parts(n_parts);

    // skeleton graph
    for(int i=0; i<n_parts; i++)
    {
        int node, parent_skeleton_node;
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %d %n", &node, &parent_skeleton_node, &read);
        //printf("%d %d %d\n", node, parent_skeleton_node, read);
        index += read;
        vox_dat->set_skeleton_node_parent(node, parent_skeleton_node);
    }

    // skeleton node matrixs
    for(int i=0; i<num_skeleton_nodes; i++)
    {
        int node;
        float x,y,z;
        float rx,ry,rz;
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %f %f %f  %f %f %f %n", &node, &x,&y,&z, &rx,&ry,&rz, &read);
        //printf("%d %f %f %f  %f %f %f %d\n", node, x,y,z, rx,ry,rz, read);
        index += read;
        vox_dat->set_skeleton_node_matrix(node, x,y,z, rx,ry,rz);
    }

    // voxel part size, dimension, color
    for(int i=0; i<n_parts; i++)
    {
        int part_num;
        int skeleton_parent_matrix; // not used?
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %d %s %n", &part_num, &skeleton_parent_matrix, str_tmp, &read);
        //printf ("%d %d %s %d\n", part_num, skeleton_parent_matrix, str_tmp, read);
        index += read;
        read_voxel_volume(str_tmp, part_num, skeleton_parent_matrix, vox_dat);
    }

    // voxel part rotation, anchor
    for (int i=0; i<n_parts; i++)
    {
        int part_num;
        float ox,oy,oz;
        float rx,ry,rz;
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %f %f %f  %f %f %f %n", &part_num, &ox, &oy, &oz, &rx, &ry, &rz, &read);
        //printf ("%d %f %f %f  %f %f %f %d\n", part_num, ox, oy, oz, rx, ry, rz, read);
        index += read;
        vox_dat->set_part_spatials(part_num, ox,oy,oz, rx,ry,rz);
    }

    if(index > size+1)
    {
        printf("voxel_skeleton_read: buffer overflow, index= %i, size= %i \n", index, size);
    }

    fclose(fp);
    delete[] str_tmp;
    delete[] buffer;
}

void read_voxel_volume(char* file_name, int part_num, int skeleton_parent_matrix, VoxBody* vox_dat)
{
    printf("Loading voxel model: %s \n", file_name);

    int size = fsize(file_name);
    char* buffer = new char[size+1];
    FILE *fp = fopen(file_name, "r"); //open file for reading

    int nbytes = fread(buffer, sizeof(char), size, fp);
    if ( nbytes != size )
    {
        printf("read_voxel_volume: failed to write %i bytes\n", nbytes);
        fclose(fp);
        delete[] buffer;
        return;       
    }
    buffer[size] = '\0';

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

    vox_dat->set_part_properties(part_num, skeleton_parent_matrix, vox_size, xdim, ydim, zdim, (bool)biaxial);
    //printf("vox: x,y,z= %i, %i, %i, size= %f biaxial=%d\n", xdim,ydim,zdim, vox_size, biaxial);

/*
    OMFG.  Use same voxel files on client/server
    Read them in same way, but do different things
*/
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
    //printf("team= %i, team rgb= %i %i %i \n", team, team_r, team_g, team_b);

    int ret;
    int vox_num = 0;
    int x,y,z,r,g,b;
    unsigned char a = 255;
    while(1)
    {
        check_for_comments(buffer, &index);
        if( buffer[index] == '\0' || index == size)
        {
            break;
        }
        if(index > size)
        {
            printf("voxel_volume_read: buffer overflow, index= %i, size= %i \n", index, size);
        }
        ret = sscanf (buffer+index, "%d %d %d  %d %d %d  %n", &x,&y,&z,&r,&g,&b, &read);

        if(ret == 0)
        {
            printf("read_voxel_volume: error ret==0 \n");
            break;
        }
        index += read;
        #ifdef DC_CLIENT
        vox_dat->set_color(part_num, x,y,z, (unsigned char)r,(unsigned char)g,(unsigned char)b, a);
        #endif
        //printf("set_color: %d - %d %d %d - %d %d %d %d\n", part_num, x,y,z, r,g,b,a);
        vox_num++;
    }

    //printf("voxels: %i \n", vox_num);
    
    fclose(fp); 
    delete[] buffer;
}
