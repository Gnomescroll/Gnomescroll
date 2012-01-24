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
            printf("load_skeleton_from_file: failed to write %i bytes\n", nbytes);
            fclose(fp);
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
            check_for_comments(buffer, &index);
            sscanf (buffer+index, "%d %s %n", &num_volumes, str_tmp, &read);
            index += read;
        }


        fclose(fp);
        delete vox_skel;
    }


};



void test_voxel_skeleton()
{
    return;
    class Voxel_loader vl;
    vl.read_skeleton("./media/voxel/test_skeleton");

    //class Voxel_skeleton a((char*) test_string);
}