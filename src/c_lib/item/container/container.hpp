#pragma once

#include <agent/constants.hpp>

#include <item/common/enum.hpp>
#include <item/common/constant.hpp>
#include <item/properties.hpp>

namespace ItemContainer
{

// init
void init_container(class ItemContainerInterface* container);

#if DC_CLIENT
// transactions
ContainerActionType alpha_action_decision_tree(int id, int slot);
ContainerActionType beta_action_decision_tree(int id, int slot);

ContainerActionType nanite_alpha_action_decision_tree(int id, int slot);
ContainerActionType nanite_beta_action_decision_tree(int id, int slot);
ContainerActionType nanite_shopping_alpha_action_decision_tree(int id, int slot);
ContainerActionType nanite_shopping_beta_action_decision_tree(int id, int slot);

ContainerActionType craft_input_alpha_action_decision_tree(int id, int slot);
ContainerActionType craft_input_beta_action_decision_tree(int id, int slot);
ContainerActionType craft_output_alpha_action_decision_tree(int id, int slot);
ContainerActionType craft_output_beta_action_decision_tree(int id, int slot);

ContainerActionType no_container_alpha_action_decision_tree();
ContainerActionType no_container_beta_action_decision_tree();
#endif

#if DC_SERVER
// transactions
ContainerActionType alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

ContainerActionType nanite_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType nanite_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType nanite_shopping_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType nanite_shopping_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

ContainerActionType craft_input_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType craft_input_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType craft_output_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot);
ContainerActionType craft_output_beta_action_decision_tree(int agent_id, int client_id, int id, int slot);

ContainerActionType no_container_alpha_action_decision_tree(int agent_id, int client_id);
ContainerActionType no_container_beta_action_decision_tree(int agent_id, int client_id);

#endif

class ItemContainerInterface
{
    public:
        int id;
        ItemContainerType type;

        int xdim;
        int ydim;

        int slot_max;
        int slot_count;
        ItemID* slot;

        int owner;
        int chunk;  // TODO -- move to subclass

        bool attached_to_agent; // true for containers permanently attached to agents (inventory, nanite)

        bool is_full()
        {
            GS_ASSERT(this->slot_count <= this->slot_max && this->slot_count >= 0);
            return (this->slot_count >= this->slot_max);
        }

        bool is_valid_slot(int slot)
        {
            GS_ASSERT(this->slot_max > 0);
            return (slot >= 0 && slot < this->slot_max);
        }

        ItemID get_item(int slot)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return NULL_ITEM;
            return this->slot[slot];
        }

        void assign_owner(int owner)
        {
            GS_ASSERT(this->attached_to_agent); // should only use this for attached containers
            this->owner = owner;
        }

        void print()
        {
            for (int i=0; i<this->slot_max; printf("%d ", this->slot[i++]));
            printf("\n");
        }

        virtual void insert_item(int slot, ItemID item_id)= 0;
        virtual void remove_item(int slot) = 0;

        virtual bool can_insert_item(int slot, ItemID item_id) = 0;

        virtual int get_stackable_slot(int item_type, int stack_size) = 0;
        virtual int get_empty_slot() = 0;

        virtual void init(int xdim, int ydim) = 0;

        virtual bool can_be_opened_by(int agent_id)
        {
            GS_ASSERT(!this->attached_to_agent || this->owner != NO_AGENT);  // agent should be assigned
            if (this->attached_to_agent && this->owner != agent_id) return false;
            return (this->owner == agent_id || this->owner == NO_AGENT);
        }
        virtual bool lock(int agent_id)
        {
            ASSERT_VALID_AGENT_ID(agent_id);
            GS_ASSERT(this->can_be_opened_by(agent_id));
            if (!this->can_be_opened_by(agent_id)) return false;
            GS_ASSERT(!this->attached_to_agent);
            if (this->attached_to_agent) return false;
            this->owner = agent_id;
            return true;
        }
        virtual bool unlock(int agent_id)
        {
            ASSERT_VALID_AGENT_ID(agent_id);
            GS_ASSERT(this->owner != NO_AGENT);
            GS_ASSERT(this->owner == agent_id);
            GS_ASSERT(!this->attached_to_agent);
            if (this->attached_to_agent) return false;
            if (this->owner != agent_id) return false;
            this->owner = NO_AGENT;
            return true;
        }

        virtual ~ItemContainerInterface()
        {
           if (this->slot != NULL) delete[] this->slot;
        }

        ItemContainerInterface(ItemContainerType type, int id)
        : id(id), type(type),
        xdim(0), ydim(0),
        slot_max(0), slot_count(0), slot(NULL),
        owner(NO_AGENT), chunk(0xffff),
        attached_to_agent(false)
        {}
};

class ItemContainer: public ItemContainerInterface
{
    public:

        bool can_insert_item(int slot, ItemID item_id)
        {
            return true;
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            for (int i=0; i<this->slot_max; i++)
            {
                if (this->slot[i] == NULL_ITEM) continue;
                if (Item::get_item_type(this->slot[i]) == item_type   // stacks
                && Item::get_stack_space(this->slot[i]) >= stack_size) // stack will fit
                    return i;
            }
            return NULL_SLOT;
        }

        int get_empty_slot()
        {
            for (int i=0; i<this->slot_max; i++)
                if (this->slot[i] == NULL_ITEM)
                    return i;
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        ItemContainer(ItemContainerType type, int id)
        : ItemContainerInterface(type, id)
        {}
};

class ItemContainerCryofreezer: public ItemContainer
{
    public:

        void insert_item(int slot, ItemID item_id);
        
        ItemContainerCryofreezer(ItemContainerType type, int id)
        : ItemContainer(type,id)
        {}
};

class ItemContainerNanite: public ItemContainerInterface
{
    public:

        int level;

        #if DC_SERVER
        int digestion_tick;
        void digest();
        #endif

        ItemID get_coins()
        {
            return this->get_item(1);
        }

        ItemID get_food()
        {
            return this->get_item(0);
        }

        void insert_coins(ItemID item_id)
        {
            this->insert_item(1, item_id);
        }

        void insert_food(ItemID item_id)
        {
            this->insert_item(0, item_id);
        }

        bool can_insert_item(int slot, ItemID item_id)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_id == NULL_ITEM) return false;
            int item_type = Item::get_item_type(item_id);
            if (slot == 0)
            {   // check against nanite's food
                return Item::get_nanite_edibility(item_type);
            }
            else if (slot == 1)
            {   // nanite coins only
                if (item_type == Item::get_item_type((char*)"nanite_coin")) return true;
                return false;
            }
            return false;   // no other slots accept insertions
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            // check food slot
            if (Item::get_item_type(this->slot[0]) == item_type
            && Item::get_stack_space(this->slot[0]) >= stack_size)
                return 0;
            // check coin slot
            if (Item::get_item_type(this->slot[1]) == item_type
            && Item::get_stack_space(this->slot[1]) >= stack_size)
                return 1;
            return NULL_SLOT;
        }

        int get_empty_slot()
        {
            // only food slot can be empty slot
            if (this->slot[0] == NULL_ITEM) return 0;
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        ItemContainerNanite(ItemContainerType type, int id)
        : ItemContainerInterface(type, id),
        level(0)
        #if DC_SERVER
        , digestion_tick(0)
        #endif
        {}
};

class ItemContainerCraftingBench: public ItemContainerInterface
{
    public:

        bool can_insert_item(int slot, ItemID item_id)
        {
            return true;
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            return true;
        }

        int get_empty_slot()
        {
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim; // +1 for the extra food slot
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        ItemContainerCraftingBench(ItemContainerType type, int id)
        : ItemContainerInterface(type, id)
        {}
};

}   // ItemContainer

#include <common/template/dynamic_multi_object_list.hpp>

namespace ItemContainer
{

ItemContainerInterface* create_item_container_interface(int type, int id)
{
    switch (type)
    {
        case AGENT_CONTAINER:
        case AGENT_TOOLBELT:
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
            return new ItemContainer((ItemContainerType)type, id);

        case AGENT_NANITE:
            return new ItemContainerNanite((ItemContainerType)type, id);

        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            return new ItemContainerCraftingBench((ItemContainerType)type, id);

        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
            return new ItemContainerCryofreezer((ItemContainerType)type, id);

        default:
            printf("ERROR -- %s -- type %d unhandled\n", __FUNCTION__, type);
            GS_ASSERT(false);
            return NULL;
    }
    return NULL;
}

const int ITEM_CONTAINER_MAX = 1024;

class ItemContainerList: public DynamicMultiObjectList<ItemContainerInterface, ITEM_CONTAINER_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:

        #if DC_CLIENT
        ItemContainerInterface* create(int type)
        {
            printf("must create item container with id\n");
            GS_ASSERT(false);
            return NULL;
        }

        ItemContainerInterface* create(int type, int id)
        {
            return DynamicMultiObjectList<ItemContainerInterface, ITEM_CONTAINER_MAX>::create(type, id);
        }
        #endif

        ItemContainerList()
        : DynamicMultiObjectList<ItemContainerInterface, ITEM_CONTAINER_MAX>(create_item_container_interface)
        {
            print_list((char*)this->name(), this);
        }
};

}
