#pragma once

#include <item/properties.hpp>

namespace HudContainer
{

class SmelterUI : public UIElement
{
    public:

        static const int cell_size = ITEM_ICON_RENDER_SIZE + 5;
        int xdim;    // grid cell size
        int ydim;

        // size of texture/render area
        float render_width;
        float render_height;

        static const int slot_size = ITEM_ICON_RENDER_SIZE;
        static const int highlight_size = ITEM_ICON_RENDER_SIZE;
        static const struct Offset highlight_offset;
        static const int cell_offset_x = 3;
        static const int cell_offset_y = 3;
        static const int cell_offset_x_right = 2;
        static const int cell_offset_y_bottom = 2;

        static const int texture_width = 512;
        static const int texture_height = 512;

        static const int output_offset_y = 26;

        float texture_offset_x;
        float texture_offset_y;

        static const int slot_label_max = 2;
        static const struct Dim slot_label_dimensions[slot_label_max];
        static const struct Offset slot_label_origins[slot_label_max];
        static const struct Offset slot_label_offsets[slot_label_max];

        bool centered;

    void draw();

    int width()
    {
        return render_width;
    }

    int height()
    {
        return render_height;
    }

    int get_grid_at(int px, int py);
    int get_slot_at(int px, int py);
    int get_grid_for_slot(int slot);

    bool point_inside(int px, int py)
    {
        return (this->get_grid_at(px,py) != NULL_SLOT);
    }

    void init()
    {
        int input_ct = ItemContainer::get_container_xdim(this->container_type) * ItemContainer::get_container_ydim(this->container_type);
        int output_ct = ItemContainer::get_container_alt_xdim(this->container_type) * ItemContainer::get_container_alt_ydim(this->container_type);
        int fuel_ct = 1;
        this->init_item_labels(input_ct + output_ct + fuel_ct);

        this->refresh_render_size();
        if (this->centered) this->center();
    }

    void center()
    {
        int x = _xresf - this->width();
        float off = 0;
        if (x % 2) off = 1;
        this->xoff = x/2 + off;
    }

    void refresh_render_size()
    {
        this->render_width = this->cell_size * this->xdim;
        this->render_height = this->cell_size * this->ydim;
    }

    void set_container_type(ItemContainerType container_type)
    {
        if (this->container_type == container_type) return;

        this->container_type = container_type;

        if (container_type == ItemContainer::name::smelter_basic)
        {
            this->texture = &SmelterTexture;
            this->texture_offset_x = 0.0f;
            this->texture_offset_y = 0.0f;
            this->xdim = 3;
            this->ydim = 2;
        }
        else
        {
            GS_ASSERT(false);
        }

        this->init();
    }

    bool in_fuel_region(int px, int py)
    {
        int slot = this->get_grid_at(px,py);
        int xslot = slot % this->xdim;
        int yslot = slot / this->xdim;
        return (xslot == 1 && yslot == 1);
    }

    bool in_input_region(int px, int py)
    {
        int slot = this->get_grid_at(px,py);
        int xslot = slot % this->xdim;
        int x = ItemContainer::get_container_xdim(this->container_type);
        int xoff = 1;
        return (xslot >= xoff && xslot < x + xoff);
    }

    bool in_output_region(int px, int py)
    {
        int x = this->cell_size * 2;
        int y = this->output_offset_y;

        x += xoff;
        y += _yres - yoff;

        return (px >= x && px < x + cell_size &&
                py >= y && py < y + cell_size);
    }

    bool in_inactive_region(int px, int py)
    {
        return (!this->in_fuel_region(px,py) &&
                !this->in_input_region(px,py) &&
                !this->in_output_region(px,py));
    }

    void draw_meter(float x, float y, float src_x, float src_y, float w, float h, float amount);

    SmelterUI() :
        xdim(1), ydim(1),
        render_width(1.0f), render_height(1.0f),
        texture_offset_x(0.0f), texture_offset_y(0.0f),
        centered(false)
    {}

    virtual ~SmelterUI()
    {
    }
};

const struct Offset SmelterUI::highlight_offset = { 3, 3 };

const struct Dim SmelterUI::slot_label_dimensions[SmelterUI::slot_label_max] = {
    { 29, 12 },
    { 29, 12 },
};
const struct Offset SmelterUI::slot_label_origins[SmelterUI::slot_label_max] = {
    { 212, 25 },
    { 212, 13 },
};
const struct Offset SmelterUI::slot_label_offsets[SmelterUI::slot_label_max] = {
    { 10, 18 },
    { 10, 18 },
};


int SmelterUI::get_grid_at(int px, int py)
{
    //pixels from upper left
    px -= xoff;
    py -= _yresf - yoff;

    if (px < 0 || px >= render_width)  return NULL_SLOT;
    if (py < 0 || py >= render_height) return NULL_SLOT;

    int xslot = px / cell_size;
    int yslot = py / cell_size;
    int slot = xslot + yslot * xdim;

    return slot;
}

int SmelterUI::get_slot_at(int px, int py)
{
    if (this->in_fuel_region(px,py)) return 0;
    if (this->in_input_region(px,py)) return 1;
    if (this->in_output_region(px,py)) return 2;
    return NULL_SLOT;
}

int SmelterUI::get_grid_for_slot(int slot)
{
    IF_ASSERT(slot == NULL_SLOT) return NULL_SLOT;
    if (slot == 0) return (this->xdim * 1) + 1; // fuel slot
    if (slot == 1) return 1;    // input slot
    if (slot == 2) return 2;    // WARNING -- output slot is not grid aligned
    return NULL_SLOT;
}

// x,y is the offset of the meter
// src_x, src_y is the offset of the meter color strip
// amount is a value between 0 and 1
void SmelterUI::draw_meter(float x, float y, float src_x, float src_y, float w, float h, float amount)
{
    // the texture should already be bound
    GL_ASSERT(GL_TEXTURE_2D, true);

    // force the amount in range
    IF_ASSERT(amount < 0.0f) amount = 0.0f;
    IF_ASSERT(amount > 1.0f) amount = 1.0f;

    float tx_min = src_x / float(this->texture_width);
    float ty_min = src_y / float(this->texture_height);
    float tx_max = tx_min + (w/float(this->texture_width));
    float ty_max = ty_min + (h/float(this->texture_height));

    float dh = h * (1.0f - amount);

    // draw from bottom of meter
    glBegin(GL_QUADS);

    glTexCoord2f(tx_min, ty_min);
    glVertex2f(x, y - dh);

    glTexCoord2f(tx_min, ty_max);
    glVertex2f(x, y - h);

    glTexCoord2f(tx_max, ty_max);
    glVertex2f(x + w, y - h);

    glTexCoord2f(tx_max, ty_min);
    glVertex2f(x + w, y - dh);

    glEnd();
}

void SmelterUI::draw()
{
    IF_ASSERT(this->texture == NULL) return;

    this->draw_name();

    if (this->container_id == NULL_CONTAINER) return;
    ItemContainer::ItemContainerUIInterface* container_ui = ItemContainer::get_container_ui(this->container_id);
    IF_ASSERT(container_ui == NULL) return;
    GS_ASSERT(Item::is_smelter(container_ui->type));
    ItemContainer::ItemContainerSmelterUI* smelter = (ItemContainer::ItemContainerSmelterUI*)container_ui;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, *this->texture);

    glColor4ub(255, 255, 255, 255);

    float w = render_width;
    float h = render_height;

    float x = xoff;
    float y = yoff;

    float tx_min = texture_offset_x;
    float ty_min = texture_offset_y;
    float tx_max = render_width/float(this->texture_width);
    float ty_max = render_height/float(this->texture_height);

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

    if (smelter != NULL)
    {
        int grid_xoff = 0;
        // background meters
        this->draw_meter(xoff + float(this->cell_size)*grid_xoff + 8.0f,
                         yoff - 2.0f, 160.0f, 2.0f, 14.0f, 102.0f, 1.0f);
        this->draw_meter(xoff + float(this->cell_size)*grid_xoff + 32.0f,
                         yoff - 2.0f, 160.0f, 2.0f, 14.0f, 102.0f, 1.0f);

        this->draw_meter(xoff + float(this->cell_size)*grid_xoff + 8.0f,
                         yoff - 2.0f, 174.0f, 2.0f, 14.0f, 102.0f, smelter->fuel);
        this->draw_meter(xoff + float(this->cell_size)*grid_xoff + 32.0f,
                         yoff - 2.0f, 188.0f, 2.0f, 14.0f, 102.0f, smelter->progress);
    }

    glDisable(GL_TEXTURE_2D);

    // draw hover highlight
    glBegin(GL_QUADS);
    glColor4ub(160, 160, 160, 128);
    int hover_slot = this->get_grid_at(mouse_x, mouse_y);

    if (hover_slot != NULL_SLOT && !this->in_inactive_region(mouse_x, mouse_y))
    {
        int w = this->highlight_size;
        int xslot = hover_slot % this->xdim;
        int yslot = hover_slot / this->xdim;

        if (this->in_output_region(mouse_x, mouse_y))
            yslot = 0;

        float x = xoff + cell_size*xslot + highlight_offset.x;
        float y = yoff - (cell_size*yslot + highlight_offset.y);

        if (this->in_output_region(mouse_x, mouse_y))
            y -= this->output_offset_y;

        glVertex2f(x,y);
        glVertex2f(x, y-w);
        glVertex2f(x+w, y-w);
        glVertex2f(x+w, y);
    }
    glEnd();

    // get data for rendering items
    struct ItemContainer::SlotMetadata* slot_metadata =
        ItemContainer::get_container_ui_slot_metadata(container_id);
    if (slot_metadata == NULL) return;
    IF_ASSERT(this->slot_label_max > smelter->slot_max) return;

    // draw durabilities
    for (int slot=0; slot<smelter->slot_max; slot++)
    {
        int grid = this->get_grid_for_slot(slot);
        int xslot = grid % this->xdim;
        int yslot = grid / this->xdim;
        const float x = xoff + cell_offset_x + cell_size*xslot;
        const float y = yoff - (cell_size*(yslot+1)) + cell_offset_y_bottom;
        draw_durability_meter(x, y, slot_size, 128, slot_metadata[slot]);
    }


    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);

    // draw slot labels
    glBindTexture(GL_TEXTURE_2D, *this->texture);
    for (int i=0; i<this->slot_label_max; i++)
    {
        if (slot_metadata[i].type != NULL_ITEM_TYPE) continue;
        int slot = this->get_grid_for_slot(i);
        int xslot = slot % this->xdim;
        int yslot = this->ydim - (slot / this->xdim);
        draw_bound_texture_sprite(
            this->xoff + cell_offset_x + cell_size*xslot + this->slot_label_offsets[i].x,
            (this->yoff - this->height()) + (cell_offset_y + cell_size*(yslot-1) + this->slot_label_offsets[i].y),
            this->slot_label_dimensions[i].x, this->slot_label_dimensions[i].y,
            -1.0f,
            float(this->slot_label_origins[i].x) / float(this->texture_width),
            float(this->slot_label_origins[i].y) / float(this->texture_height),
            float(this->slot_label_dimensions[i].x) / float(this->texture_width),
            float(this->slot_label_dimensions[i].y) / float(this->texture_height));
    }

    //draw items
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);
    glBegin(GL_QUADS);
    for (int slot=0; slot<smelter->slot_max; slot++)
    {
        ItemType item_type = slot_metadata[slot].type;
        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        int grid = this->get_grid_for_slot(slot);
        int xslot = grid % this->xdim;
        int yslot = grid / this->xdim;

        if (slot == smelter->slot_max-1)
            yslot = 0;

        float x = xoff + cell_offset_x + cell_size*xslot;
        float y = yoff - (cell_offset_y + cell_size*yslot);

        if (slot == smelter->slot_max-1)
            y -= this->output_offset_y;

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

    glDisable(GL_TEXTURE_2D);

    // draw item_labels
    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    for (int slot=0; slot<smelter->slot_max; slot++)
    {
        int stack = slot_metadata[slot].stack_size;
        int charges = slot_metadata[slot].charges;
        HudText::Text* text = &this->item_labels[slot];

        int grid = this->get_grid_for_slot(slot);
        int xslot = grid % this->xdim;
        int yslot = grid / this->xdim;

        if (slot == smelter->slot_max-1)
            yslot = 0;

        float x = xoff + cell_size*(xslot+1) - cell_offset_x_right - text->get_width();
        float y = yoff - (cell_size*(yslot+1) - cell_offset_y_bottom - text->get_height());
        if (slot == smelter->slot_max-1)
            y -= this->output_offset_y;

        draw_slot_numbers(text, x, y, stack, charges);
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    //draw_tracking_pixel(xoff,yoff);
    glColor4ub(255, 255, 255, 255);

}

}   // HudContainer
