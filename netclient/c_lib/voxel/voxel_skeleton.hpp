#pragma once

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>

#include <stdio.h>

static const char* test_string = "3\n4 5\n #comment \n 6 5 \x00 \n";

class Voxel_skeleton
{
    private:
        void check_for_comments(char* s, int* index)
        {   
            while(s[*index] == ' ' || s[*index] == '\t' || s[*index] == '\n') (*index)++;
            if(s[*index] != '#' || s[*index] == 0) return;
            printf("Comment at: %i \n", *index);

            while(s[*index] != '\n' && s[*index] != 0) (*index)++;
            //if(s[*index] != 0) (*index)++; //scanf skips newlines anyways
            //this function will always return on a new line or null
            printf("skipped to: %i \n", *index);
        }

    public:

    Voxel_volume** voxel_volume_list;
    int voxel_volume_num;
    int* voxel_volume;


    Voxel_skeleton(char* string)
    {
        init_from_string(string);
    }

    void init_from_string(char* s)
    {

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
    }

};

void test_voxel_skeleton()
{
    class Voxel_skeleton a((char*) test_string);
}