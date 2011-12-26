#include "voxel_render.hpp"



void Voxel_render_list::register_voxel_volume(Voxel_volume* vv)
{
    if(num_elements >= VOXEL_RENDER_LIST_SIZE)
    {
        printf("Voxel_render_list Error: number of voxel models exceeds VOXEL_RENDER_LIST_SIZE \n");
        return;
    }
    num_elements++;

    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL)
        {
            render_list[i] = vv;
            printf("Added voxel volume %i \n", i);
            break;
        }
    }

    vv->voxel_render_list = this;
}

void Voxel_render_list::unregister_voxel_volume(Voxel_volume* vv)
{
    num_elements--;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == vv)
        {
            render_list[i] = NULL;
            printf("Removed voxel volume %i \n", i);
            break;
        }
    }
}