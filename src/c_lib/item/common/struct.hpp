#pragma once


namespace Item
{

class SynthesizerItem
{
    public:

    int item_type;
    int cost;

    int xslot;
    int yslot;

    SynthesizerItem()
    {
        item_type = NULL_ITEM_TYPE;
        cost = NULL_COST;
        
        xslot = NULL_SLOT;
        yslot = NULL_SLOT;
    }
};

class CraftingRecipe
{
    public:

    int id;
    int output; //item type
    int output_stack;
    int reagent_num;
    int reagent[CRAFT_BENCH_INPUTS_MAX];
    int reagent_count[CRAFT_BENCH_INPUTS_MAX];

    // temporary state information
    bool available;

    CraftingRecipe()
    {
        init();
    }

    void init()
    {
        id = NULL_CRAFTING_RECIPE;
        output = NULL_ITEM_TYPE;
        output_stack = 1;
        reagent_num = 0;
        for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; i++)
        {
            reagent[i] = NULL_ITEM_TYPE;
            reagent_count[i] = 1;
        }
        available = true;
    }

    //CraftingRecipe& operator=(const CraftingRecipe& other)
    //{
        //if (this != &other) // protect against invalid self-assignment
        //{
            //this->id = other.id;
            //this->output = other.output;
            //this->output_stack = other.output_stack;
            //this->reagent_num = other.reagent_num;
            //this->available = other.available;
            
            //for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; i++)
            //{
                //this->reagent[i] = other.reagent[i];
                //this->reagent_count[i] = other.reagent_count[i];
            //}
        //}
        //// by convention, always return *this
        //return *this;
    //}
};


class SmeltingRecipe
{
    public:

    int id;
    int output_num;
    int output[SMELTER_OUTPUTS_MAX]; //item type
    int output_stack[SMELTER_OUTPUTS_MAX];
    int reagent_num;
    int reagent[SMELTER_INPUTS_MAX];
    int reagent_count[SMELTER_INPUTS_MAX];
    int creation_time;  // total ticks to synthesize
    
    bool available;

    SmeltingRecipe()
    {
        this->init();
    }

    void init()
    {
        this->id = NULL_SMELTING_RECIPE;
        for (int i=0; i<SMELTER_OUTPUTS_MAX; this->output[i++] = NULL_ITEM_TYPE);
        for (int i=0; i<SMELTER_OUTPUTS_MAX; this->output_stack[i++] = 1);
        this->output_num = 0;
        this->reagent_num = 0;
        for (int i=0; i<SMELTER_INPUTS_MAX; i++)
        {
            this->reagent[i] = NULL_ITEM_TYPE;
            this->reagent_count[i] = 1;
        }
        this->available = true;
        this->creation_time = DEFAULT_SMELTING_RECIPE_CREATION_TIME;
    }
};

}
