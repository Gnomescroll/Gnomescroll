#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <common/random.hpp>
#include <item/common/constants.hpp>
#include <item/common/struct.hpp>

namespace Item
{

class ItemDropEntry
{
    private:
        float get_cumulative_probability()
        {
            float p = 0.0f;
            for (int i=0; i<this->n_drops; i++)
                p += this->probability[i];
            return p;
        }

    public:

        int n_drops;
        int item_type;
        int* amount;
        float* probability;

    void set_drops(int n)
    {
        GS_ASSERT(n > 0);
        if (n < 0) return;
        GS_ASSERT(this->amount == NULL);
        GS_ASSERT(this->probability == NULL);
        this->n_drops = n;
        this->amount = (int*)calloc(n, sizeof(int));
        this->probability = (float*)calloc(n, sizeof(float));
    }

    void add_drop(int amount, float probability)
    {
        GS_ASSERT(amount > 0);
        if (amount <= 0) return;
        GS_ASSERT(probability > 0.0f && probability < 1.001f);

        GS_ASSERT(this->get_cumulative_probability() + probability < 1.001f);

        // check that this drop has not been recorded
        for (int i=0; i<this->n_drops; i++)
            GS_ASSERT(this->amount[i] != amount);

        for (int i=0; i<this->n_drops; i++)
            if (this->amount[i] <= 0)
            {    // free slot
                this->amount[i] = amount;
                this->probability[i] = probability;
                return;
            }
        GS_ASSERT(false);
    }

    void drop_item(struct Vec3 position, float vx,  float vy, float vz, randFloat vx_func, randFloat vy_func, randFloat vz_func);
    class Item* drop_item();

    ItemDropEntry() :
    n_drops(0), item_type(NULL_ITEM_TYPE), amount(NULL), probability(NULL)
    {}

    ~ItemDropEntry()
    {
        if (this->amount != NULL) free(this->amount);
        if (this->probability != NULL) free(this->probability);
    }
};

class ItemDrop
{
    private:
        int n_drops;
        int max_drops;
        class ItemDropEntry* drop;

        int get_or_create_drop_enty(int item_type)
        {
            GS_ASSERT(item_type != NULL_ITEM_TYPE);
            if (item_type == NULL_ITEM_TYPE) return -1;

            for (int i=0; i<this->max_drops; i++)
            {
                if (this->drop[i].item_type == item_type)
                    return i;
                else
                if (this->drop[i].item_type == NULL_ITEM_TYPE)
                {
                    this->drop[i].item_type = item_type;
                    this->n_drops++;
                    return i;
                }
            }
            GS_ASSERT(false);
            return -1;
        }

        class ItemDropEntry* get_or_create_drop_entry(int item_type)
        {
            GS_ASSERT(this->drop != NULL);
            if (this->drop == NULL) return NULL;

            GS_ASSERT(item_type != NULL_ITEM_TYPE);
            if (item_type == NULL_ITEM_TYPE) return NULL;

            int i = 0;
            for (; i<this->max_drops; i++)
            {
                if (this->drop[i].item_type == item_type)
                    break;
                else
                if (this->drop[i].item_type == NULL_ITEM_TYPE)
                {
                    this->drop[i].item_type = item_type;
                    this->n_drops++;
                    break;
                }
            }
            GS_ASSERT(i < this->max_drops);
            if (i >= this->max_drops) return NULL;
            return &this->drop[i];
        }

    public:

        class Item** dropped_items;
        int n_dropped_items;

        // creation state configuration
        randFloat vx_func;
        randFloat vy_func;
        randFloat vz_func;
        float vx,vy,vz;

        // config
        void set_max_drop_types(int n)
        {
            GS_ASSERT(n > 0);
            if (n <= 0) return;
            GS_ASSERT(this->drop == NULL);
            GS_ASSERT(this->max_drops == 0)
            this->max_drops = n;
            this->drop = new ItemDropEntry[n];
            this->dropped_items = (class Item**)calloc(n, sizeof(class Item*));
        }

        void set_max_drop_amounts(const char* item_name, int n)
        {
            int item_type = get_item_type(item_name);
            GS_ASSERT(item_type != NULL_ITEM_TYPE);
            if (item_type == NULL_ITEM_TYPE) return;
            class ItemDropEntry* drop = this->get_or_create_drop_entry(item_type);
            GS_ASSERT(drop != NULL);
            if (drop == NULL) return;

            drop->set_drops(n);
        }

        void add_drop(const char* item_name, int amount, float probability)
        {
            int item_type = get_item_type(item_name);
            GS_ASSERT(item_type != NULL_ITEM_TYPE);
            if (item_type == NULL_ITEM_TYPE) return;
            class ItemDropEntry* drop = this->get_or_create_drop_entry(item_type);
            GS_ASSERT(drop != NULL);
            if (drop == NULL) return;
            drop->add_drop(amount, probability);
        }

        void add_drop_range(const char* item_name, int lower_amount, int higher_amount, float probability)
        {    // gives all drop amounts in a range equal probability
            GS_ASSERT(lower_amount < higher_amount);
            if (lower_amount >= higher_amount) return;
            float each_probability = probability / (higher_amount - lower_amount);
            for (int i=lower_amount; i<higher_amount; i++)
                this->add_drop(item_name, i, each_probability);
        }

        void drop_item(Vec3 position);

        bool is_loaded()
        {
            return (n_drops >= 0 && max_drops >= 0 && drop != NULL);
        }

    ItemDrop() :
        n_drops(0), max_drops(0), drop(NULL),
        dropped_items(NULL), n_dropped_items(0),
        vx_func(NULL), vy_func(NULL), vz_func(NULL),
        vx(0.0f), vy(0.0f), vz(0.0f)
    {}

    ~ItemDrop()
    {
        if (this->drop != NULL) delete[] this->drop;
        if (this->dropped_items != NULL) free(this->dropped_items);
    }

};

}   // Item
