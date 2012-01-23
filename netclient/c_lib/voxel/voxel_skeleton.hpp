#pragma once

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>

#include <stdio.h>

static const char* test_string = "3\n4 5\n #comment \n 6 5 \x00 \n";


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
            if(s[*index] != '#' || s[*index] == 0) return;
            while(s[*index] != '\n' && s[*index] != 0) (*index)++;
            goto jmp;
        }

    public:
    //Voxel_volume** voxel_volume_list;
    //int voxel_volume_num;
    //int* voxel_volume;

    Voxel_loader() {}

    void load_skeleton_from_file(char* file_name)
    {

        int size = fsize(file_name);

        char* buffer = new char[size+1];

        FILE *fp = fopen(file_name, "r"); //open file for reading

        int nbytes = fread(buffer, sizeof(char), size, fp);
        if ( nbytes != size )
        {
            printf("load_skeleton_from_file: failed to write %zu bytes\n", nbytes);
            return;       
        }
        buffer[size] = NULL;

        printf("skeleton file size= %i \n", size);

        char* str_tmp = new char[512];


        int num_volumes;

        int index = 0;
        int read;

        //read in number of voxel volumes
        check_for_comments(buffer, &index);
        sscanf (buffer+index, "%d %n", &num_volumes, &read);
        index += read;

        printf("num_volumes= %i \n", num_volumes);


    }

    void init_from_string(char* s)
    {
        /*
        int index = 0;
        int read;
     
        check_for_comments(s, &index);
             
        sscanf (s+index, "%d %n", &voxel_volume_num, &read);
        printf("voxel_volume_num= %i \n", voxel_volume_num);
        printf("read %i bytes\n", read);
        index += read;

        check_for_comments(s, &index);

        int n1, n2;
        sscanf (s+index, "%d %d %n", &n1, &n2, &read);
        printf("num1= %i num2= %i \n", n1, n2);
        printf("read %i bytes\n", read);
        index += read;

        check_for_comments(s, &index);

        sscanf (s+index, "%d %d %n", &n1, &n2, &read);
        printf("num1= %i num2= %i \n", n1, n2);
        printf("read %i bytes\n", read);
        */
    }

};

class Voxel_skeleton
{
    private:

    public:

    Voxel_volume** voxel_volume_list;
    int voxel_volume_num;
    int* voxel_volume;


    Voxel_skeleton(char* string)
    {
    }

    void init_from_string(char* s)
    {
    }

};

void test_voxel_skeleton()
{
    return;
    class Voxel_loader vl;
    vl.load_skeleton_from_file("./media/voxel/test_skeleton");

    //class Voxel_skeleton a((char*) test_string);
}