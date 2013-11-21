/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "item_drop_alt.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <item/item.hpp>

namespace Item
{

void ItemDropConfig::save_to_file()
{
    GS_ASSERT(max > 0);
    GS_ASSERT(this->get_name_from_id != NULL);

    const char fmt[] = DATA_PATH "%s_drop_dat.txt";
    char* name = this->name;
    IF_ASSERT(name == NULL) name = (char*)"";
    size_t len = strlen(fmt) + strlen(this->name) - 2;
    char* filename = (char*)malloc((len+1) * sizeof(char));
    sprintf(filename, fmt, name);

    FILE* fp = fopen(filename, "w");
    free(filename);
    IF_ASSERT(fp == NULL) return;

    for (int i=0; i<this->max; i++)
    {
        int index = this->meta_drop_table[i].index;
        if (index < 0) continue;

        int num_drop = this->meta_drop_table[i].num_drop;
        if (num_drop == 0) continue;

        if (this->get_name_from_id != NULL)
        {
            char* drop_entry_name = (char*)this->get_name_from_id(i);
            if (drop_entry_name == NULL)
                drop_entry_name = (char*)"NULL";
            fprintf(fp, "%s: %s \n", this->name, drop_entry_name);
        }

        for (int j=0; j<num_drop; j++)
        {
            struct ItemDropTable* cidt = &this->item_drop_table[index+j];

            float average = 0.0f;

            for (int k=0; k<cidt->drop_entries; k++)
                average += cidt->item_drop_num[k]*cidt->drop_probabilities[k];

            fprintf(fp,"\t%d: %s \n", j, get_item_name(cidt->item_type));
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
                    cidt->item_drop_num[k]*cidt->drop_probabilities[k],
                    cdrop);
            }
        }
    }

    fclose(fp);
}

void ItemDropConfig::end()
{
    GS_ASSERT(this->max > 0);
    for (int i=0; i<this->max; i++)
    {
        int index = this->meta_drop_table[i].index;
        if (index < 0) continue;

        int num_drop = this->meta_drop_table[i].num_drop;
        if (num_drop == 0) continue;

        for (int j=0; j<num_drop; j++)
        {
            struct ItemDropTable* cidt = &this->item_drop_table[index+j];

            float total = 0.0f;

            for (int k=1; k<cidt->drop_entries; k++)
                total += cidt->drop_probabilities[k];

            GS_ASSERT(total < 0.999f || cidt->drop_entries <= 2);
            if (total > 0.999f && cidt->drop_entries > 2) //means non-zero drop rate of less than 1%
            {
                printf("Drop Dat Error: Item total probabilities exceed 0.99 (p = %f)\n", total);
                char* item_name = (char*)get_item_name(cidt->item_type);
                if (item_name == NULL) item_name = (char*)"NULL";
                printf("Drop error for item: %s \n", item_name);
                continue;
            }

            cidt->drop_probabilities[0] = 1.0f - total;
            if (cidt->drop_probabilities[0] < 0.0f)
                cidt->drop_probabilities[0] = 0;    // correct any negative value due to precision error

            total = 0.0f;
            for (int k=0; k<cidt->drop_entries; k++)
            {
                total += cidt->drop_probabilities[k];
                cidt->drop_cumulative_probabilities[k] = total;
            }
        }
    }
}

void ItemDropConfig::def_drop(const char* name)
{
    GS_ASSERT(this->get_id_from_name != NULL);
    if (this->get_id_from_name == NULL) return;
    int id = this->get_id_from_name(name);
    GS_ASSERT(id >= 0 && id < this->max);
    if (id < 0 || id >= this->max) return;
    this->_current_drop_id = id;
}

void ItemDropConfig::add_drop(const char* item_name, int drop_entries)
{
    GS_ASSERT(drop_entries > 0);
    if (drop_entries <= 0) return;

    drop_entries++; // why? reserved "no drop" slot?

    ItemType item_type = get_item_type(item_name);
    int id = this->_current_drop_id;

    if (this->add_drop_callback != NULL)
        this->add_drop_callback(id);

    if (this->meta_drop_table[id].index < 0)
    {
        GS_ASSERT(this->meta_drop_table[id].num_drop == 0);
        this->meta_drop_table[id].index = this->item_drop_table_index;
    }

    this->meta_drop_table[id].num_drop++;

    struct ItemDropTable* cide;
    cide = &this->item_drop_table[this->item_drop_table_index];
    this->item_drop_table_index++;

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

    this->_current_cide = cide;
    this->_current_drop_num = 1;  // 0th drop slot is reserved for "no drop" probability
}

void ItemDropConfig::set_drop(float drop_probability, int drops)
{
    GS_ASSERT(this->_current_drop_num < this->_current_cide->drop_entries);
    if (this->_current_drop_num >= this->_current_cide->drop_entries)
    {
        printf("Item Drop Dat Error: current item id exceeds drop entries\n");
        return;
    }

    this->_current_cide->drop_probabilities[this->_current_drop_num] = drop_probability;
    this->_current_cide->item_drop_num[this->_current_drop_num] = drops;

    this->_current_drop_num++;
}

void ItemDropConfig::drop_always(const char* item_name, int drops)
{
    this->add_drop(item_name, 1);
    this->set_drop(1.0f, drops);
}

void ItemDropConfig::drop_always(const char* item_name)
{
    this->drop_always(item_name, 1);
}

bool ItemDropConfig::has_drop_defined(const char* name)
{
    if (this->get_id_from_name == NULL) return false;
    int id = this->get_id_from_name(name);
    GS_ASSERT(id >= 0 && id < this->max);
    if (id < 0 || id >= this->max) return false;
    if (this->meta_drop_table[id].index >= 0) return true;
    return false;
}

}   // Item
