#pragma once

#include <t_map/t_properties.hpp>
#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_map
{

struct CubeItemDropMeta
{
    unsigned short index;   //start index
    unsigned short num_drop;    //number of possible drops
};

struct CubeItemDropTable
{
    int item_type;
    int max_drops;  //number of entries
    float* drop_probabilities;      //index into probability table
};


int probability_table_index = 0;
float probability_table[4096];


struct CubeItemDropMeta meta_drop_table[256];

int item_drop_table_index = 0;
struct CubeItemDropTable item_drop_table[512];


void save_drop_dat_to_file()
{
    FILE *fp = fopen("screenshot/drop_dat","w");
    if(!fp) GS_ABORT();

    for(int i=0; i<256; i++)
    {
        int num_drop = meta_drop_table[i].num_drop;
        if(num_drop == 0) continue;

        fprintf(fp,"Block: %s \n", get_cube_name(i));

        int index = meta_drop_table[i].index;

        for(int j=0; j<num_drop; j++)
        {
            struct CubeItemDropTable* cidt = &item_drop_table[index+j];

            float average = 0;
            float _clast = 0.;
            for(int k=0; k<cidt->max_drops; k++)
            {
                _clast = cidt->drop_probabilities[k] - _clast;
                average += k*_clast;
                _clast = cidt->drop_probabilities[k];
            }

            fprintf(fp,"\t%d: %s \n", j, Item::get_item_name(cidt->item_type) );

            fprintf(fp,"\tavg= %2.4f max= %i \n", average , cidt->max_drops-1);


            float clast = 0;
            float dprob = 0;
            float cdrop = 0.0;
            for(int k=0; k<cidt->max_drops; k++)
            {
                float p = cidt->drop_probabilities[k];
                dprob = p - clast;
                cdrop += k*dprob;
                //cumulative probability, probability, drop average, cumulative drop average
                fprintf(fp,"\t\t%i: %2.4f \t %2.4f \t %2.2f \t %2.2f \n", k ,p, dprob, dprob*k, cdrop);
                clast = cidt->drop_probabilities[k];
            }

        }


    }
    fclose(fp);


}

#if DC_SERVER
void handle_block_drop(int x, int y, int z, int block_type)
{
    for (int i=0; i < meta_drop_table[block_type].num_drop; i++)
    {

        CubeItemDropTable* cidt = &item_drop_table[i+meta_drop_table[block_type].index];

        float p = randf();

        for (int j=0; j < cidt->max_drops; j++)
        {
            //check each drop until commulative probability table hit
            if (p <= cidt->drop_probabilities[j])
            {
                j = cidt->max_drops; //terminate inner loop  
                if (j==0) continue; //no drop

                for (int k=0; k<j; k++)
                {
                    const float mom = 2.0f;
                    x = (float)x + 0.5f + randf()*0.33;
                    y = (float)y + 0.5f + randf()*0.33;
                    z = (float)z + 0.05f;
                    ItemParticle::create_item_particle(cidt->item_type, x, y, z, 
                        (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom); 
                }

            }
        }
    }
}
#endif

int _current_drop_block_id;

void def_drop(const char* block_name)
{
    //printf("err1 \n");
    int block_id = t_map::dat_get_cube_id(block_name);
    //printf("err2 \n");
    _current_drop_block_id = block_id;
}

void add_drop(const char* item_name, float mean, float falloff, int max_drops)
{
    assert(max_drops > 0);
    max_drops++;

    int item_type = Item::dat_get_item_type(item_name);
    int block_id = _current_drop_block_id;

    cube_list[block_id].item_drop = true;

    if ( meta_drop_table[block_id].num_drop == 0)
        meta_drop_table[block_id].index = item_drop_table_index;
        
    meta_drop_table[block_id].num_drop++;

    struct CubeItemDropTable* cide;
    cide = &item_drop_table[item_drop_table_index];
    item_drop_table_index++;

    cide->item_type = item_type;
    cide->max_drops = max_drops;
    cide->drop_probabilities = &probability_table[probability_table_index];
    probability_table_index += max_drops;

    //procedure, calculate mean, adjust probabilites down to mean, and normalize 0 drop

    double p[max_drops];

    p[0] = 1.0f;
    for (int i=1; i<max_drops; i++) p[i] = p[i-1] * falloff;
    
    //normalize distribution
    double sum = 0.0;
    for (int i=0; i<max_drops; i++) sum += p[i];
    for (int i=0; i<max_drops; i++) p[i] /= sum;
    
    //sum = 0.0;
    //for (int i=0; i<max_drops; i++) sum += p[i];
    //printf("Droptable: normalized sum= %f \n", (float)sum);

    double _mean_drop = 0.0;
    for (int i=0; i<max_drops; i++) _mean_drop += i*p[i];
    
    printf("Droptable: mean_drop= %f \n", ((float) _mean_drop) );

    
    //for (int i=0; i<max_drops; i++) cide->drop_probabilities[i] = (float) p[i];

    for (int i=0; i<max_drops; i++)
    {
        double sum = 0.0;
        for (int j=0; j <= i; j++) sum += p[j];
        cide->drop_probabilities[i] = (float) sum;
    }
}


}   // t_map
