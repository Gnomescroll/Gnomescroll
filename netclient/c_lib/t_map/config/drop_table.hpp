#pragma once

#include <t_map/t_properties.hpp>
#include <item/properties.hpp>

namespace t_map
{

struct CubeItemDropMeta
{
    unsigned short index;	//start index
    unsigned short num_drop;	//number of possible drops
};

struct CubeItemDropTable
{
	int item_id;
	int max_drops;	//number of entries
	float* drop_probabilities;		//index into probability table
};


int probability_table_index = 0;
float probability_table[4096];


struct CubeItemDropMeta meta_drop_table[256];

int item_drop_table_index = 0;
struct CubeItemDropTable item_drop_table[512];


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
	int item_id = Item::dat_get_item_id(item_name);
	int block_id = _current_drop_block_id;

	if( meta_drop_table[block_id].num_drop == 0)
	{
		meta_drop_table[block_id].num_drop = 1;
		meta_drop_table[block_id].index = item_drop_table_index;
	}
	else
	{
		meta_drop_table[block_id].num_drop++;
	}

	struct CubeItemDropTable* cide;
	cide = &item_drop_table[item_drop_table_index++];
	item_drop_table_index++;

	cide->item_id = item_id;
	cide->max_drops = max_drops;
	cide->drop_probabilities = &probability_table[probability_table_index];
	probability_table_index += max_drops;


	//procedure, calculate mean, adjust probabilites down to mean, and normalize 0 drop

	double p[max_drops];

	double tmp = 1.0;
	for(int i=0; i<max_drops; i++)
	{
		p[i] = tmp;
		tmp *= falloff;
	}
	
	//normalize distribution
	double sum = 0.0;
	for(int i=0; i<max_drops; i++) sum += p[i];
	for(int i=0; i<max_drops; i++) p[i] = p[i] / sum;
	
	sum = 0.0;
	for(int i=0; i<max_drops; i++) sum += p[i];
	printf("Normalized sum= %f \n", (float)sum);

	double _mean_drop = 0.0;
	for(int i=1; i<max_drops; i++)
	{
		_mean_drop += p[i];
	}
	printf("mean_drop= %f \n", ((float) mean) );



	for(int i=1; i<max_drops; i++) cide->drop_probabilities[i] = (float) p[i];

		printf("err3\n");
}

}