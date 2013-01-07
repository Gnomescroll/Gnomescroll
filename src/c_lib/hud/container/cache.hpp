#pragma once

#include <hud/hud.hpp>
#include <hud/container/_interface.hpp>
#include <hud/container/texture.hpp>
#include <SDL/SDL_functions.hpp>

namespace HudContainer
{

class CacheUI : public AgentInventoryUI
{
    public:

    void init()
    {
        GS_ASSERT(this->stack_numbers == NULL);
        GS_ASSERT(this->container_type != NULL_CONTAINER_TYPE);
        GS_ASSERT(this->container_type == ItemContainer::name::premium_cache);
        
        this->xdim = ItemContainer::get_container_xdim(this->container_type);
        this->ydim = ItemContainer::get_container_ydim(this->container_type);

        // create HudText objects needed for stack rendering
        int max = this->xdim * this->ydim;
        IF_ASSERT(max <= 0) return;
        this->stack_numbers = new HudText::Text[max];

        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->stack_numbers[i];
            t->set_format("%d");
            t->set_format_extra_length(11 + 1 - 2);
            t->set_color(Color(255,255,255,255));
            t->set_depth(-0.1f);
        }
    }

    CacheUI()
    {
        this->slot_background_border_color = Color(183, 110, 121);
    }
};

}   // HudContainer
