#pragma once

#include <item/properties.hpp>

namespace t_hud
{

//221x147

const int ITEM_PRICE_MAX_LENGTH = 4;

void u_dot(float x, float y);

class AgentNaniteUI : public UIElement
{
    public:

    static const int cell_size = 37;    // pixel dimension

    static const int nanite_xdim = 4;
    static const int nanite_ydim = 4;
    
    static const int shopping_xdim = 2;
    static const int shopping_ydim = 4;

    static const int xdim = nanite_xdim + shopping_xdim;
    static const int ydim = (nanite_ydim > shopping_ydim) ? nanite_ydim : shopping_ydim;

    static const int level = 0;    //nanite level

    // size of texture/render area
    static const float render_width = cell_size * xdim;
    static const float render_height = cell_size * ydim;

    static const float slot_size = 32;
    static const float cell_offset_x = 3;
    static const float cell_offset_y = 3;
    static const float cell_offset_x_right = 2;
    static const float cell_offset_y_bottom = 2;

    static const float nanite_slot_render_offset_x = 58.0f;
    static const float nanite_slot_render_offset_y = 114.0f;

    HudText::Text* prices;
    HudText::Text* stacks;

    void draw();

    int width()
    {
        return render_width;
    }

    int height()
    {
        return render_height;
    }

    int get_slot_at(int px, int py);
    int get_grid_at(int px, int py);

    bool point_inside(int px, int py)
    {
        return (this->get_grid_at(px,py) != NULL_SLOT);
    }

    bool in_nanite_region(int px, int py);
    bool in_shopping_region(int px, int py);
    bool in_coins_region(int px, int py);

    void init()
    {
        GS_ASSERT(this->prices == NULL);
        int max = shopping_xdim * shopping_ydim - 1;    // last slot is coins
        this->prices = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->prices[i];
            t->set_format((char*) "%d");
            t->set_format_extra_length(ITEM_PRICE_MAX_LENGTH + 1 - 2);
            t->set_color(255,255,255,255);    // some kind of red
            t->set_depth(-0.1f);
        }

        // stacks only for food/coins
        GS_ASSERT(this->stacks == NULL);
        max = 2;
        this->stacks = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->stacks[i];
            t->set_format((char*) "%d");
            t->set_format_extra_length(STACK_COUNT_MAX_LENGTH + 1 - 2);
            t->set_color(255,255,255,255);    // some kind of red
            t->set_depth(-0.1f);
        }
    }

    void set_container_type(int container_type)
    {
        this->container_type = container_type;
    }

    AgentNaniteUI() : prices(NULL), stacks(NULL)
    {}

    ~AgentNaniteUI()
    {
        if (this->prices != NULL) delete[] this->prices;
        if (this->stacks != NULL) delete[] this->stacks;
    }
};

bool AgentNaniteUI::in_nanite_region(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return false;
    
    int xslot = slot % xdim;
    int yslot = slot / xdim;
    GS_ASSERT(xslot >= 0 && xslot < xdim);
    GS_ASSERT(yslot >= 0 && yslot < ydim);

    if (xslot < 0 || xslot >= nanite_xdim) return false;
    if (yslot < 0 || yslot >= nanite_ydim) return false;
    return true;
}

bool AgentNaniteUI::in_shopping_region(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return false;
    
    int xslot = slot % xdim;
    int yslot = slot / xdim;
    GS_ASSERT(xslot >= 0 && xslot < xdim);
    GS_ASSERT(yslot >= 0 && yslot < ydim);
    
    if (xslot == xdim-1 && yslot == ydim-1) return false;  // coins slot
    if (xslot < nanite_xdim || xslot >= xdim) return false;
    if (yslot < 0 || yslot >= ydim) return false;
    return true;
}

bool AgentNaniteUI::in_coins_region(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return false;
    if (slot == xdim*ydim-1) return true;
    return false;
}

int AgentNaniteUI::get_grid_at(int px, int py)
{
    px -= xoff;
    py -= _yresf - yoff;

    if (px < 0 || px >= render_width) return NULL_SLOT;
    if (py < 0 || py >= render_height) return NULL_SLOT;

    int xslot = px / cell_size;
    int yslot = py / cell_size;
    int slot = xslot + yslot * xdim;

    GS_ASSERT(slot < xdim*ydim);
    
    return slot;
}

int AgentNaniteUI::get_slot_at(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return NULL_SLOT;
    
    if (this->in_nanite_region(px,py)) return 0;  // food slot
    if (this->in_coins_region(px,py)) return 1;

    // convert grid slot to shopping slot (0-indexed)

    int xslot = slot % xdim;
    int yslot = slot / xdim;
    GS_ASSERT(xslot >= nanite_xdim && xslot < xdim);
    GS_ASSERT(yslot >= 0 && yslot < ydim);
    
    xslot -= nanite_xdim;
    slot = xslot + yslot * shopping_xdim;
    return slot;
}

//221x147
void AgentNaniteUI::draw()
{
    glDisable(GL_DEPTH_TEST); // move render somewhere
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GS_ASSERT(NaniteTexture != 0);
    glBindTexture(GL_TEXTURE_2D, NaniteTexture);

    glColor4ub(255, 255, 255, 255);

    float w = render_width;
    float h = render_height;

    float x = xoff;
    float y = yoff;

    float tx_min = 0.0f;
    float ty_min = 0.0f;
    float tx_max = w/512.0f;
    float ty_max = h/512.0f;

    //draw background
    glBegin(GL_QUADS);

    glTexCoord2f(tx_min, ty_min);
    glVertex2f(x, y);

    glTexCoord2f(tx_min, ty_max);
    glVertex2f(x,y-h);

    glTexCoord2f(tx_max, ty_max);
    glVertex2f(x+w, y-h);

    glTexCoord2f(tx_max, ty_min);
    glVertex2f(x+w, y);
    
    // draw a slot on the nanite for indication
    // use FOOD slot in top right corner of texture
    tx_min = (512.0f - cell_size)/512.0f;
    ty_min = 0.0f;
    tx_max = tx_min + cell_size/512.0f;
    ty_max = ty_min + cell_size/512.0f;

    // these coordinates place it at hardcoded special location
    //x = xoff + (nanite_slot_render_offset_x);
    //y = yoff - (nanite_slot_render_offset_y + slot_size);

    // bottom right corner
    x = xoff + (nanite_xdim - 1) * cell_size;
    y = yoff - ((nanite_ydim - 1) * cell_size);

    w = cell_size;
    h = cell_size;
    
    glTexCoord2f(tx_min, ty_min);
    glVertex2f(x, y);

    glTexCoord2f(tx_min, ty_max);
    glVertex2f(x,y-h);

    glTexCoord2f(tx_max, ty_max);
    glVertex2f(x+w, y-h);

    glTexCoord2f(tx_max, ty_min);
    glVertex2f(x+w, y);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_grid_at(mouse_x, mouse_y);
    if (hover_slot != NULL_SLOT)
    {
        int w = slot_size;
        int xslot,yslot;

        if (this->in_nanite_region(mouse_x, mouse_y))
        {
            xslot = nanite_xdim - 1;
            yslot = nanite_ydim - 1;
        }
        else
        {
            xslot = hover_slot % this->xdim;
            yslot = hover_slot / this->xdim;
        }
        float x = xoff + cell_size*xslot + cell_offset_x;
        float y = yoff - (cell_size*yslot + cell_offset_y);

        glVertex2f(x,y);
        glVertex2f(x, y-w);
        glVertex2f(x+w, y-w);
        glVertex2f(x+w, y);
    }
    glEnd();

    if (this->container_id == NULL_CONTAINER) return;

    // draw food
    ItemContainer::ItemContainerNaniteUI* container = (ItemContainer::ItemContainerNaniteUI*)ItemContainer::get_container_ui(this->container_id);
    if (container == NULL) return;

    int coins = 0;
    if (container->get_coin_type() != NULL_ITEM_TYPE) coins = container->get_coin_stack();

    // greyscale items

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    GS_ASSERT(TextureSheetLoader::GreyScaleItemTexture != 0);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::GreyScaleItemTexture);

    glBegin(GL_QUADS);

    //draw unavailable store items
    bool any_available;
    for (int xslot=0; xslot<shopping_xdim; xslot++)
    for (int yslot=0; yslot<shopping_ydim; yslot++)
    {
        if (xslot == shopping_xdim-1 && yslot == shopping_ydim-1) continue;    // this is the last slot, put money here

        int cost;
        int item_type = Item::get_nanite_store_item(level, xslot, yslot, &cost);
        if (coins >= cost)
        {
            any_available = true;
            continue; // we can afford it; move on
        }
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + cell_size*nanite_xdim + cell_size*xslot + cell_offset_x;
        const float y = yoff - (cell_size*yslot + cell_offset_y);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0/iw)*(tex_id % iiw);
        const float ty_min = (1.0/iw)*(tex_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x,y-w);

        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y-w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y);
    }
    glEnd();

    // item sheet items
    if (any_available)
    {
        GS_ASSERT(TextureSheetLoader::ItemSheetTexture != 0);
        glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::ItemSheetTexture);

        glBegin(GL_QUADS);
        //draw available store items
        for (int xslot=0; xslot<shopping_xdim; xslot++)
        for (int yslot=0; yslot<shopping_ydim; yslot++)
        {
            if (xslot == shopping_xdim-1 && yslot == shopping_ydim-1) continue;    // this is the last slot, put money here

            int cost;
            int item_type = Item::get_nanite_store_item(level, xslot, yslot, &cost);
            if (coins < cost) continue; // we can't afford it; move on
            if (item_type == NULL_ITEM_TYPE) continue;
            int tex_id = Item::get_sprite_index_for_type(item_type);

            const float x = xoff + cell_size*nanite_xdim + cell_size*xslot + cell_offset_x;
            const float y = yoff - (cell_size*yslot + cell_offset_y);
            
            const float w = slot_size;
            const float iw = 16.0f; // icon_width
            const int iiw = 16; // integer icon width

            const float tx_min = (1.0/iw)*(tex_id % iiw);
            const float ty_min = (1.0/iw)*(tex_id / iiw);
            const float tx_max = tx_min + 1.0/iw;
            const float ty_max = ty_min + 1.0/iw;

            glTexCoord2f(tx_min, ty_min);
            glVertex2f(x, y);

            glTexCoord2f(tx_min, ty_max);
            glVertex2f(x,y-w);

            glTexCoord2f(tx_max, ty_max);
            glVertex2f(x+w, y-w);

            glTexCoord2f(tx_max, ty_min);
            glVertex2f(x+w, y);
        }
        glEnd();
    }

    int food_item_type = container->get_food_type();
    if (food_item_type != NULL_ITEM_TYPE)
    {
        int food_sprite_id = Item::get_sprite_index_for_type(food_item_type);
        const float x = xoff + (nanite_xdim-1)*cell_size + cell_offset_x;
        const float y = yoff - ((nanite_ydim-1)*cell_size + cell_offset_y);
        //const float x = xoff + nanite_slot_render_offset_x;
        //const float y = yoff - nanite_slot_render_offset_y;
        
        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0/iw)*(food_sprite_id % iiw);
        const float ty_min = (1.0/iw)*(food_sprite_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glBegin(GL_QUADS);
        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x,y-w);

        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y-w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y);
        glEnd();
    }
    
    // draw coins
    int coin_item_type = container->get_coin_type();
    if (coin_item_type != NULL_ITEM_TYPE)
    {
        int coin_sprite_id = Item::get_sprite_index_for_type(coin_item_type);
        const float x = xoff + (xdim-1)*cell_size + cell_offset_x;
        const float y = yoff - ((ydim-1)*cell_size + cell_offset_y);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0/iw)*(coin_sprite_id % iiw);
        const float ty_min = (1.0/iw)*(coin_sprite_id / iiw);
        const float tx_max = tx_min + 1.0/iw;
        const float ty_max = ty_min + 1.0/iw;

        glBegin(GL_QUADS);
        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x,y-w);

        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y-w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    ////draw text for item cost in upper right

    GS_ASSERT(this->stacks != NULL);
    GS_ASSERT(this->prices != NULL);

    HudFont::start_font_draw();
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    for (int xslot=0; xslot<shopping_xdim; xslot++)
    for (int yslot=0; yslot<shopping_ydim; yslot++)
    {
        if (xslot == shopping_xdim-1 && yslot == shopping_ydim-1) continue;  // skip last slot, reserved

        int cost;
        int item_type = Item::get_nanite_store_item(level, xslot, yslot, &cost);
        if (item_type == NULL_ITEM_TYPE) continue;

        GS_ASSERT(count_digits(cost) < ITEM_PRICE_MAX_LENGTH);

        const int slot = yslot*shopping_xdim + xslot;
        GS_ASSERT(slot < shopping_xdim*shopping_ydim-1);
        HudText::Text* text = &this->prices[slot];
        text->update_formatted_string(1, cost);

        const float x = xoff + nanite_xdim*cell_size + (xslot+1)*cell_size - cell_offset_x_right - text->get_width();
        const float y = yoff - ((yslot+1)*cell_size - cell_offset_y_bottom - text->get_height());

        text->set_position(x,y);
        text->draw();
    }

    // draw food stack
    int food_stack_size = container->get_food_stack();
    GS_ASSERT(count_digits(food_stack_size) < STACK_COUNT_MAX_LENGTH);
    if (food_stack_size > 1)
    {
        this->stacks[0].update_formatted_string(1, food_stack_size);

        const float x = xoff + nanite_xdim*cell_size - cell_offset_x_right - this->stacks[0].get_width();
        const float y = yoff - (nanite_ydim*cell_size - cell_offset_y_bottom - this->stacks[0].get_height());
        
        this->stacks[0].set_position(x,y);
        this->stacks[0].draw();
    }
    
    // draw coin stack
    int coin_stack_size = container->get_coin_stack();
    GS_ASSERT(count_digits(coin_stack_size) < STACK_COUNT_MAX_LENGTH);
    if (coin_stack_size > 1)
    {
        this->stacks[1].update_formatted_string(1, coin_stack_size);

        const float x = xoff + xdim*cell_size - cell_offset_x_right - this->stacks[1].get_width();
        const float y = yoff - (ydim*cell_size - cell_offset_y_bottom - this->stacks[1].get_height());

        this->stacks[1].set_position(x,y);
        this->stacks[1].draw();
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glColor4ub(255, 255, 255, 255);

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}


void u_dot(float x, float y)
{
    /*
        Draw dot in upper left corner
    */
    glDisable(GL_TEXTURE_2D);
    glColor4ub(255, 0, 0, 255);

    float p = 1.0;
    glBegin(GL_QUADS);

    glVertex2f(x-p,y+p);
    glVertex2f(x+p, y+p);
    glVertex2f(x+p, y-p);
    glVertex2f(x-p, y-p);

    glEnd();
}

}
