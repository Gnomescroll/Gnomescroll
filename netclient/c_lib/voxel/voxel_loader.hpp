#pragma once

#include <c_lib/voxel/voxel_skeleton.hpp>

#include <sys/stat.h>

off_t fsize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

class Voxel_loader
{
    private:

        //this function will always return on a new line or null
        void check_for_comments(char* s, int* index)
        {   
            jmp:
            while(s[*index] == ' ' || s[*index] == '\t' || s[*index] == '\n') (*index)++;
            if(s[*index] != '#' || s[*index] == '\0') return;
            while(s[*index] != '\n' && s[*index] != '\0') (*index)++;
            goto jmp;
        }

    public:
    //Voxel_volume** voxel_volume_list;
    //int voxel_volume_num;
    //int* voxel_volume;

    Voxel_loader() {}

    void read_skeleton(char* file_name)
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
        buffer[size] = NULL;

        //printf("skeleton file size= %i \n", size);

        char* str_tmp = new char[512];


        int num_volumes;

        int index = 0;
        int read;

        //read in number of voxel volumes
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %n", &num_volumes, &read);
        index += read;

        printf("num_volumes= %i \n", num_volumes);


        Voxel_skeleton *vox_skel = new Voxel_skeleton(num_volumes);

        for(int i=0; i<num_volumes; i++)
        {
            int volume_num;
            check_for_comments(buffer, &index);
            sscanf (buffer+index, "%d %s %n", &volume_num, str_tmp, &read);
            index += read;
            read_voxel_volume(str_tmp, &vox_skel->voxel_volume_list[volume_num] );
        }

        for(int i=0; i<num_volumes; i++)
        {
            int x1, x2;
            check_for_comments(buffer, &index);
            sscanf (buffer+index, "%d %d %n", &x1, &x2, &read);
            index += read;
            
            vox_skel->skeleton_tree[2*i+0] = x1;
            vox_skel->skeleton_tree[2*i+1] = x2;
        }

        for(int i=0; i<num_volumes; i++)
        {
            float x,y,z;
            float rx,ry,rz;

            check_for_comments(buffer, &index);
            sscanf (buffer+index, "%f %f %f  %f %f %f %n", &x,&y,&z, &rx,&ry,&rz, &read);

            printf("x,y,z= %.2f %.2f %.2f rx,ry,rz= %.2f %.2f %.2f \n", x,y,z, rx,ry,rz );

            mat4_euler_rotation_and_translation( x,y,z, rx,ry,rz);

            //vox_skel->voxel_volume_list[i].local_matrix = mat4_euler_rotation( rx,ry,rz );
            //vox_skel->voxel_volume_list[i].local_matrix.v[4] = vec4_init(x,y,z,1.0);

            index += read;
        }

        if(index > size)
        {
            printf("voxel_skeleton_read: buffer overflow, index= %i, size= %i \n", index, size);
        }

        fclose(fp);
        delete vox_skel;
        delete[] str_tmp;
        delete[] buffer;
    }

    void read_voxel_volume(char* file_name, class Voxel_volume* vox)
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
        buffer[size] = NULL;

        int index = 0;
        int read;

        //read in size
        int xdim,ydim,zdim;

        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %d %d %n", &xdim,&ydim,&zdim, &read);
        index += read;

        //read in voxsize
        float vox_size;

        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%f %n", &vox_size, &read);
        index += read;

        printf("vox: x,y,z= %i, %i, %i, size= %f \n", xdim,ydim,zdim, vox_size);

        vox->init(xdim,ydim,zdim,vox_size);

        int ret;
        int vox_num = 0;
        int x,y,z,r,g,b;

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

            vox->set(x,y,z, r,g,b,0);
            vox_num++;
            //set voxel model
        }

        printf("voxels: %i \n", vox_num);

        fclose(fp); 
        delete[] buffer;
    }


};



void test_voxel_skeleton()
{
    //static int count = 0;
    class Voxel_loader vl;
    vl.read_skeleton( (char*)"./media/voxel/test_skeleton");

    //class Voxel_skeleton a((char*) test_string);
}