#pragma once

#include <t_map/t_properties.hpp>
#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_map
{

void def_drop(const char* block_name);
void add_drop(const char* item_name, int drop_entries);
void set_drop(float drop_probability, int drops);
void drop_always(const char* item_name);

struct CubeItemDropMeta
{
    unsigned short index;   //start index
    unsigned short num_drop;    //number of possible drops
};

struct CubeItemDropTable
{
    int item_type;
    int drop_entries;                //number of entries
    float* drop_probabilities;      //index into probability table
    float* drop_cumulative_probabilities;
    int* item_drop_num;                 //number of items to drop
};


int _table_index = 0;
float probability_table[4096];
float cumulative_probability_table[4096];
int item_drop_num_table[4096];

struct CubeItemDropMeta meta_drop_table[256];

int item_drop_table_index = 0;
struct CubeItemDropTable item_drop_table[512];


void save_drop_dat_to_file()
{
    FILE *fp = fopen("screenshot/drop_dat.txt","w");
    if (!fp) GS_ABORT();

    for (int i=0; i<256; i++)
    {
        int num_drop = meta_drop_table[i].num_drop;
        if (num_drop == 0) continue;

        fprintf(fp,"Block: %s \n", get_cube_name(i));

        int index = meta_drop_table[i].index;

        for (int j=0; j<num_drop; j++)
        {
            struct CubeItemDropTable* cidt = &item_drop_table[index+j];

            float average = 0;

            for (int k=0; k<cidt->drop_entries; k++)
            {
                average += cidt->item_drop_num[k]*cidt->drop_probabilities[k];
            }

            fprintf(fp,"\t%d: %s \n", j, Item::get_item_name(cidt->item_type));

            fprintf(fp,"\tavg= %2.4f \n", average);

            float cdrop = 0.0f;
            for (int k=0; k<cidt->drop_entries; k++)
            {
                cdrop += cidt->item_drop_num[k]*cidt->drop_probabilities[k];
                //cumulative probability, probability, drop average, cumulative drop average
                fprintf(fp,"\t\t%i: %2.4f \t %2.4f \t %2.2f \t %2.2f \n", 
                    cidt->item_drop_num[k],
                    cidt->drop_probabilities[k],
                    cidt->drop_cumulative_probabilities[k], 
                    cidt->item_drop_num[k]*cidt->drop_probabilities[k]
                    ,cdrop);

            }

        }


    }

    fclose(fp);
}

void end_drop_dat()
{
    for (int i=0; i<256; i++)
    {
        int num_drop = meta_drop_table[i].num_drop;
        if (num_drop == 0) continue;

        int index = meta_drop_table[i].index;

        for (int j=0; j<num_drop; j++)
        {
            struct CubeItemDropTable* cidt = &item_drop_table[index+j];

            float total;

            total = 0.0f;
            for (int k=1; k<cidt->drop_entries; k++)
            {
                total += cidt->drop_probabilities[k];
            }

            if (total > 0.99f && cidt->drop_entries > 2) //means non-zero drop rate of less than 1%
            {
                printf("Block Drop Dat Error: Item total probabilities exceed 0.99 (p = %f)\n", total);
                printf("Drop error for item: %s \n", Item::get_item_name(cidt->item_type) );
                printf("drop entries: %i \n",cidt->drop_entries);
                GS_ABORT();
            }

            cidt->drop_probabilities[0] = 1.0f - total;
			if (cidt->drop_probabilities[0] < 0.0f)
				cidt->drop_probabilities[0] = 0;	// correct any negative value due to precision error

            //float fa[cidt->drop_entries];

            total = 0.0f;
            for (int k=0; k<cidt->drop_entries; k++)
            {
                total += cidt->drop_probabilities[k];
                cidt->drop_cumulative_probabilities[k] = total;
            }
            //fprintf(fp,"\t%d: %s \n", j, Item::get_item_name(cidt->item_type));

            //fprintf(fp,"\tavg= %2.4f max= %i \n", average , cidt->max_drops-1);

/*
            float clast = 0;
            float dprob = 0;
            float cdrop = 0.0;
            for (int k=0; k<cidt->drop_entries; k++)
            {
                float p = cidt->drop_probabilities[k];
                dprob = p - clast;
                cdrop += k*dprob;
                //cumulative probability, probability, drop average, cumulative drop average
                fprintf(fp,"\t\t%i: %2.4f \t %2.4f \t %2.2f \t %2.2f \n", k ,p, dprob, dprob*k, cdrop);
                clast = cidt->drop_probabilities[k];
            }
*/
        }


    }
}


#if DC_SERVER
void handle_block_drop(int x, int y, int z, int block_type)
{
    for (int i=0; i < meta_drop_table[block_type].num_drop; i++)
    {
        CubeItemDropTable* cidt = &item_drop_table[i+meta_drop_table[block_type].index];
        float p = randf();

        if (p <= cidt->drop_cumulative_probabilities[0]) continue;

        for (int j=1; j < cidt->drop_entries; j++)
        {
            //check each drop until commulative probability table hit
            if (p <= cidt->drop_cumulative_probabilities[j])
            {
                for (int k=0; k<cidt->item_drop_num[j]; k++)
                {
                    const float mom = 2.0f;
                    x = (float)((float)x + 0.5f + randf()*0.33f);
                    y = (float)((float)y + 0.5f + randf()*0.33f);
                    z = (float)((float)z + 0.05f);
                    ItemParticle::create_item_particle(cidt->item_type, x, y, z, 
                        (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom); 
                }
                break;
            }
        }
    }
}
#endif

int _current_drop_block_id;
struct CubeItemDropTable* _current_cide;
int _current_drop_num = 1;

void def_drop(const char* block_name)
{
    //printf("err1 \n");
    int block_id = t_map::dat_get_cube_id(block_name);
    //printf("err2 \n");
    _current_drop_block_id = block_id;
}

void add_drop(const char* item_name, int drop_entries)
{
    drop_entries += 1;
    GS_ASSERT(drop_entries > 0);

    int item_type = Item::dat_get_item_type(item_name);
    int block_id = _current_drop_block_id;

    cube_list[block_id].item_drop = true;

    if (meta_drop_table[block_id].num_drop == 0)
        meta_drop_table[block_id].index = item_drop_table_index;
        
    meta_drop_table[block_id].num_drop++;

    struct CubeItemDropTable* cide;
    cide = &item_drop_table[item_drop_table_index];
    item_drop_table_index++;

    cide->item_type = item_type;
    cide->drop_entries = drop_entries;
    cide->drop_probabilities = &probability_table[_table_index];
    cide->drop_cumulative_probabilities = &cumulative_probability_table[_table_index];
    cide->item_drop_num = &item_drop_num_table[_table_index];
    _table_index += drop_entries;

    for (int i=0; i<drop_entries; i++)
    {
        cide->drop_probabilities[i] = 1.0f;
        cide->item_drop_num[i] = 0;
    }

    _current_cide = cide;
    _current_drop_num = 1;	// 0th drop slot is reserved for "no drop" probability

}


void set_drop(float drop_probability, int drops)
{
    if (_current_drop_num == _current_cide->drop_entries)
    {
        printf("Item Drop Dat Error: current item id exceeds drop entries \n");
        GS_ABORT();
    }

    _current_cide->drop_probabilities[_current_drop_num] = drop_probability;
    _current_cide->item_drop_num[_current_drop_num] = drops;

    _current_drop_num++;
}

/*
void normalize_drops_to(float total_probability)
{	// normalize all drops defined for current entry, and scale to total_probability	
	GS_ASSERT(_current_drop_num > 0);
	if (_current_drop_num <= 0) return;

	GS_ASSERT(total_probability <= 1.0f);
	if (total_probability > 1.0f)
		printf("WARNING: Normalizing drops to cumulative probability %f\n", total_probability);

	// get cumulative probability
	float p = 0.0f;
	for (int i=1; i<_current_drop_num; i++)
		p += _current_cide->drop_probabilities[i];

	GS_ASSERT(p > 0.0f);
	if (p <= 0.0f) return;
		
	// calculate normalization factor
	p = total_probability / p;
	
	// apply
	for (int i=1; i<_current_drop_num; i++)
		_current_cide->drop_probabilities[i] *= p;
}
*/

void drop_always(const char* item_name, int drops)
{
    add_drop(item_name, 1);
    set_drop(1.0f, drops);
}
void drop_always(const char* item_name) { drop_always(item_name, 1); }

}   // t_map
