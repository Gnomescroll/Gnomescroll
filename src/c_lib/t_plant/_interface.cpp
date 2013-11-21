/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <t_plant/_data.hpp>
#include <t_plant/plant_example.hpp>

namespace t_plant
{

void init()
{
    init_data();

    register_plant(0, "plant_example", sizeof(struct PlantExampleStruct));
    register_plant_function("plant_example", "init",     (fptr_void) &plant_example_init);
    register_plant_function("plant_example", "teardown", (fptr_void) &plant_example_teardown);
    register_plant_function("plant_example", "tick",     (fptr_void) &plant_example_tick );
}

void teardown()
{
    teardown_data();
}

void tick()
{
    //plant_array.tick();

    //printf("test\n");

    int type_max = plant_type_array.PLANT_TYPE_MAX;
    for (int type_id=0; type_id<type_max; type_id++ )
    {
        if (plant_type_array.array[type_id].struct_size == 0)
            continue;

        fptr_void tick_func_ptr = plant_callbacks.get_callback(type_id, "tick");
        void* data_ptr = NULL;

        for (int i=0; i<plant_array.index; i++)
        {
            if (plant_array.array[i].type_id == type_id)
            {
                data_ptr = plant_array.array[i].data_ptr;
                tick_func_ptr(data_ptr);
            }
        }
    }

}



void create_plant(const Vec3i& position, int type_id)
{
    GS_ASSERT(type_id == 0);
    plant_placement_data.position = position;
    plant_array.element_create(type_id);
}

}   // t_plant
