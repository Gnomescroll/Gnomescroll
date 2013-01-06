#pragma once

#include <item/properties.hpp>

namespace HudContainer
{

//221x147

const int SYNTHESIZER_ITEM_COST_MAX_STRLEN = 10;

class AgentSynthesizerUI : public UIElement
{
    public:

        static const int cell_size = 37;    // pixel dimension

        int xdim;
        int ydim;

        int shopping_xdim;
        int shopping_ydim;

        // size of texture/render area
        float render_width;
        float render_height;

        static const float slot_size;
        static const float cell_offset_x;
        static const float cell_offset_y;
        static const float cell_offset_x_right;
        static const float cell_offset_y_bottom;

        HudText::Text* prices;
        HudText::Text coin_stack;

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
        return (this->get_slot_at(px,py) != NULL_SLOT);
    }

    bool in_shopping_region(int px, int py);
    bool in_coins_region(int px, int py);

    void init()
    {
        this->shopping_xdim = ItemContainer::get_container_alt_xdim(ItemContainer::name::synthesizer);
        this->shopping_ydim = ItemContainer::get_container_alt_ydim(ItemContainer::name::synthesizer);
        this->xdim = shopping_xdim + ItemContainer::get_container_xdim(ItemContainer::name::synthesizer);
        this->ydim = shopping_ydim;
        
        this->render_width = this->cell_size * this->xdim;
        this->render_height = this->cell_size * this->ydim;

        
        GS_ASSERT(this->prices == NULL);
        int max = shopping_xdim * shopping_ydim;    // last slot is coins
        this->prices = new HudText::Text[max];
        for (int i=0; i<max; i++)
        {
            HudText::Text* t = &this->prices[i];
            t->set_format("%d");
            t->set_format_extra_length(SYNTHESIZER_ITEM_COST_MAX_STRLEN + 1 - 2);
            t->set_color(Color(255,255,255,255));
            t->set_depth(-0.1f);
        }

        coin_stack.set_format("%d");
        coin_stack.set_format_extra_length(11 + 1 - 2);
        coin_stack.set_color(Color(255,255,255,255));
        coin_stack.set_depth(-0.1f);
    }

    void set_container_type(ItemContainerType container_type)
    {
        this->container_type = container_type;
    }

    AgentSynthesizerUI() :
        xdim(0), ydim(0),
        shopping_xdim(0), shopping_ydim(0),
        render_width(0.0f), render_height(0.0f), prices(NULL)
    {}

    virtual ~AgentSynthesizerUI()
    {
        if (this->prices != NULL) delete[] this->prices;
    }
};

// constants
const float AgentSynthesizerUI::slot_size = 32;
const float AgentSynthesizerUI::cell_offset_x = 3;
const float AgentSynthesizerUI::cell_offset_y = 3;
const float AgentSynthesizerUI::cell_offset_x_right = 2;
const float AgentSynthesizerUI::cell_offset_y_bottom = 2;

bool AgentSynthesizerUI::in_shopping_region(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return false;
    
    int xslot = slot % xdim;
    int yslot = slot / xdim;
    GS_ASSERT(xslot >= 0 && xslot < xdim);
    GS_ASSERT(yslot >= 0 && yslot < ydim);
    
    return (xslot >= 0 && xslot < shopping_xdim
         && yslot >= 0 && yslot < shopping_ydim);
}

bool AgentSynthesizerUI::in_coins_region(int px, int py)
{
    if (this->get_grid_at(px,py) == this->xdim*this->ydim-1)
        return true;    // last slot
    return false;
}

int AgentSynthesizerUI::get_grid_at(int px, int py)
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

int AgentSynthesizerUI::get_slot_at(int px, int py)
{
    int slot = this->get_grid_at(px,py);
    if (slot == NULL_SLOT) return NULL_SLOT;
    
    if (this->in_coins_region(px,py)) return 0;

    // convert grid slot to shopping slot (0-indexed)

    int xslot = slot % xdim;
    int yslot = slot / xdim;
    if (xslot >= shopping_xdim) return NULL_SLOT;
    if (yslot >= shopping_ydim) return NULL_SLOT;
    
    slot = xslot + yslot * shopping_xdim;
    return slot;
}

//221x147
void AgentSynthesizerUI::draw()
{
    // draw name
    this->draw_name();
    
    //GS_ASSERT(SynthesizerTexture != 0);
    if (SynthesizerTexture == 0) return;
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GS_ASSERT(SynthesizerTexture != 0);
    glBindTexture(GL_TEXTURE_2D, SynthesizerTexture);

    glColor4ub(255, 255, 255, 255);

    float w = render_width;
    float h = render_height;
    if (w*h == 0.0f) return;

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
    
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    if (this->in_shopping_region(mouse_x, mouse_y)
     || this->in_coins_region(mouse_x, mouse_y))
    {
        int w = slot_size;
        int xslot,yslot;

        int hover_slot = this->get_grid_at(mouse_x, mouse_y);
        xslot = hover_slot % this->xdim;
        yslot = hover_slot / this->xdim;
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
    using ItemContainer::ItemContainerSynthesizerUI;
    ItemContainerSynthesizerUI* container = 
        (ItemContainerSynthesizerUI*)ItemContainer::get_container_ui(this->container_id);
    if (container == NULL) return;

    int coins = 0;
    if (container->get_coin_type() != NULL_ITEM_TYPE)
        coins = container->get_coin_stack();

    // greyscale items

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    GS_ASSERT(TextureSheetLoader::item_texture_sheet_loader->greyscale_texture != 0);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->greyscale_texture);

    glBegin(GL_QUADS);

    //draw unavailable store items
    bool any_available = false;
    for (int xslot=0; xslot<shopping_xdim; xslot++)
    for (int yslot=0; yslot<shopping_ydim; yslot++)
    {
        int cost;
        int item_type = Item::get_synthesizer_item(xslot, yslot, &cost);
        if (coins >= cost)
        {
            any_available = true;
            continue; // we can afford it; move on
        }
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + cell_size*xslot + cell_offset_x;
        const float y = yoff - (cell_size*yslot + cell_offset_y);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0f/iw)*(tex_id % iiw);
        const float ty_min = (1.0f/iw)*(tex_id / iiw);
        const float tx_max = tx_min + 1.0f/iw;
        const float ty_max = ty_min + 1.0f/iw;

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
        GS_ASSERT(TextureSheetLoader::item_texture_sheet_loader->texture != 0);
        glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

        glBegin(GL_QUADS);
        //draw available store items
        for (int xslot=0; xslot<shopping_xdim; xslot++)
        for (int yslot=0; yslot<shopping_ydim; yslot++)
        {
            int cost;
            int item_type = Item::get_synthesizer_item(xslot, yslot, &cost);
            if (coins < cost) continue; // we can't afford it; move on
            if (item_type == NULL_ITEM_TYPE) continue;
            int tex_id = Item::get_sprite_index_for_type(item_type);

            const float x = xoff + cell_size*xslot + cell_offset_x;
            const float y = yoff - (cell_size*yslot + cell_offset_y);
            
            const float w = slot_size;
            const float iw = 16.0f; // icon_width
            const int iiw = 16; // integer icon width

            const float tx_min = (1.0f/iw)*(tex_id % iiw);
            const float ty_min = (1.0f/iw)*(tex_id / iiw);
            const float tx_max = tx_min + 1.0f/iw;
            const float ty_max = ty_min + 1.0f/iw;

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

        const float tx_min = (1.0f/iw)*(coin_sprite_id % iiw);
        const float ty_min = (1.0f/iw)*(coin_sprite_id / iiw);
        const float tx_max = tx_min + 1.0f/iw;
        const float ty_max = ty_min + 1.0f/iw;

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

    GS_ASSERT(this->prices != NULL);

    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    for (int xslot=0; xslot<shopping_xdim; xslot++)
    for (int yslot=0; yslot<shopping_ydim; yslot++)
    {
        int cost;
        int item_type = Item::get_synthesizer_item(xslot, yslot, &cost);
        if (item_type == NULL_ITEM_TYPE) continue;

        GS_ASSERT(count_digits(cost) <= SYNTHESIZER_ITEM_COST_MAX_STRLEN);

        const int slot = yslot*shopping_xdim + xslot;
        GS_ASSERT(slot < shopping_xdim*shopping_ydim);
        HudText::Text* text = &this->prices[slot];
        if (cost <= 0)
            text->set_text("???");
        else
            text->update_formatted_string(1, cost);

        const float x = xoff + (xslot+1)*cell_size - cell_offset_x_right - text->get_width();
        const float y = yoff - ((yslot+1)*cell_size - cell_offset_y_bottom - text->get_height());

        text->set_position(x,y);
        text->draw();
    }

    // draw coin stack
    int coin_stack_size = container->get_coin_stack();
    GS_ASSERT(count_digits(coin_stack_size) < STACK_COUNT_MAX_LENGTH);
    if (coin_stack_size > 1)
    {
        this->coin_stack.update_formatted_string(1, coin_stack_size);

        const float x = xoff + xdim*cell_size - cell_offset_x_right - this->coin_stack.get_width();
        const float y = yoff - (ydim*cell_size - cell_offset_y_bottom - this->coin_stack.get_height());

        this->coin_stack.set_position(x,y);
        this->coin_stack.draw();
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glColor4ub(255, 255, 255, 255);

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}

}
