#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <common/random.hpp>
#include <item/common/constants.hpp>

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
        ItemType item_type;
        int* amount;
        float* probability;

    void set_drops(int n)
    {
        IF_ASSERT(n < 0) return;
        GS_ASSERT(this->amount == NULL);
        GS_ASSERT(this->probability == NULL);
        this->n_drops = n;
        this->amount = (int*)calloc(n, sizeof(int));
        this->probability = (float*)calloc(n, sizeof(float));
    }

    void add_drop(int amount, float probability)
    {
        IF_ASSERT(amount <= 0) return;
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

    void drop_item(struct Vec3 position, const Vec3& velocity, randFloat vx_func, randFloat vy_func, randFloat vz_func);
    class Item* drop_item();

    ItemDropEntry() :
        n_drops(0), item_type(NULL_ITEM_TYPE), amount(NULL), probability(NULL)
    {}

    ~ItemDropEntry()
    {
        free(this->amount);
        free(this->probability);
        this->amount = NULL;
        this->probability = NULL;
    }
};

class ItemDrop
{
    private:
        int n_drops;
        int max_drops;
        class ItemDropEntry* drop;

    int get_or_create_drop_enty(ItemType item_type)
    {
        IF_ASSERT(item_type == NULL_ITEM_TYPE) return -1;

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

    class ItemDropEntry* get_or_create_drop_entry(ItemType item_type)
    {
        IF_ASSERT(this->drop == NULL) return NULL;
        IF_ASSERT(item_type == NULL_ITEM_TYPE) return NULL;

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
        IF_ASSERT(i >= this->max_drops) return NULL;
        return &this->drop[i];
    }

    public:
        class Item** dropped_items;
        int n_dropped_items;
        // creation state configuration
        randFloat vx_func;
        randFloat vy_func;
        randFloat vz_func;
        Vec3 velocity;

    // config
    void set_max_drop_types(int n)
    {
        IF_ASSERT(n <= 0) return;
        GS_ASSERT(this->drop == NULL);
        GS_ASSERT(this->max_drops == 0)
        this->max_drops = n;
        this->drop = new ItemDropEntry[n];
        this->dropped_items = (class Item**)calloc(n, sizeof(class Item*));
    }

    void set_max_drop_amounts(const char* item_name, int n)
    {
        ItemType item_type = get_item_type(item_name);
        IF_ASSERT(item_type == NULL_ITEM_TYPE) return;
        class ItemDropEntry* drop = this->get_or_create_drop_entry(item_type);
        IF_ASSERT(drop == NULL) return;
        drop->set_drops(n);
    }

    void add_drop(const char* item_name, int amount, float probability)
    {
        ItemType item_type = get_item_type(item_name);
        IF_ASSERT(item_type == NULL_ITEM_TYPE) return;
        class ItemDropEntry* drop = this->get_or_create_drop_entry(item_type);
        IF_ASSERT(drop == NULL) return;
        drop->add_drop(amount, probability);
    }

    void add_drop_range(const char* item_name, int lower_amount, int higher_amount, float probability)
    {    // gives all drop amounts in a range equal probability
        IF_ASSERT(lower_amount >= higher_amount) return;
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
        velocity(vec3_init(0))
    {}

    ~ItemDrop()
    {
        delete[] this->drop;
        free(this->dropped_items);
        this->drop = NULL;
        this->dropped_items = NULL;
    }
};

}   // Item
