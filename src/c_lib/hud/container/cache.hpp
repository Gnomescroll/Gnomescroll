/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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
        GS_ASSERT(this->container_type != NULL_CONTAINER_TYPE);
        GS_ASSERT(this->container_type == ItemContainer::name::premium_cache);

        this->xdim = ItemContainer::get_container_xdim(this->container_type);
        this->ydim = ItemContainer::get_container_ydim(this->container_type);

        this->init_item_labels(this->xdim * this->ydim);
    }

    CacheUI()
    {
        this->slot_background_border_color = Color(183, 110, 121);
    }
};

}   // HudContainer
