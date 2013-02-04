#pragma once

#include <hud/container/ui_element.hpp>
#include <SDL/draw_functions.hpp>

namespace HudContainer
{

class EquipmentUI: public UIElement
{
    public:

        struct Offset
        {
            int x,y;
        };

        struct Dim
        {
            int x,y;
        };

        static const int height = 147;
        static const int width = 136;

        static const int texture_width = 256;
        static const int texture_height = 256;

        static const int slot_size = 37;
        static const int render_xdim = 3;
        static const int render_ydim = 3;
        static const int render_dim = render_xdim * render_ydim;

        static const int slots_width = render_xdim * slot_size;
        static const int slots_height = render_ydim * slot_size;

        static const int highlight_size = 34;
        static const int slot_highlight_offset = 1;

        static const int item_size = 32;
        static const int slot_item_offset = 2;

        static const struct Offset slot_label_offsets[N_EQUIPMENT_TYPES];
        static const struct Offset slot_label_origins[N_EQUIPMENT_TYPES];
        static const struct Dim slot_label_dimensions[N_EQUIPMENT_TYPES];

        static const struct Offset slot_area_offset;

        int xdim;
        int ydim;

        const static EquipmentType render_slot_equipment_types[render_dim];  // render_slot -> equipment_type
        int render_slot_map[render_dim];                                       // render_slot -> slot
        EquipmentType* slot_equipment_types;                                  // slot        -> equipment_type

    int get_render_slot_at(int px, int py)
    {
        px -= this->xoff + this->slot_area_offset.x;
        // px,py have origin at top left. our coordinates have
        // origin bottom left, so we need to flip ours
        py -= _yresf - (this->yoff - this->slot_area_offset.y);
        int xslot = px / this->slot_size;
        int yslot = py / this->slot_size;
        if (xslot < 0 || xslot >= this->render_xdim) return NULL_SLOT;
        if (yslot < 0 || yslot >= this->render_ydim) return NULL_SLOT;
        int slot = yslot * this->render_xdim + xslot;
        if (this->render_slot_equipment_types[slot] == NULL_EQUIPMENT_TYPE)
            return NULL_SLOT;
        return slot;
    }

    virtual int get_slot_at(int px, int py)
    {
        int slot = this->get_render_slot_at(px, py);
        if (slot == NULL_SLOT) return NULL_SLOT;
        return this->render_slot_map[slot];
    }

    virtual bool point_inside(int px, int py)
    {
        float y = _yresf - this->yoff;
        return (px >= this->xoff && px < this->xoff + width &&
                py >= y && py < y + height);
    }

    int get_render_slot_for_equipment_type(EquipmentType type, int offset)
    {
        IF_ASSERT(type == NULL_EQUIPMENT_TYPE) return NULL_SLOT;
        int ct = 0;
        for (int i=0; i<this->render_dim; i++)
            if (this->render_slot_equipment_types[i] == type)
            {
                if (ct == offset) return i;
                ct++;
            }
        return NULL_SLOT;
    }

    virtual void set_container_id(ItemContainerID container_id)
    {
        using ItemContainer::ItemContainerEquipmentUI;
        ItemContainerEquipmentUI* container_ui = (ItemContainerEquipmentUI*)ItemContainer::get_container_ui(container_id);
        IF_ASSERT(container_ui == NULL) return;
        GS_ASSERT(this->slot_equipment_types == NULL);
        const int size = this->xdim*this->ydim;
        GS_ASSERT(container_ui->xdim*container_ui->ydim == size);
        this->slot_equipment_types = (EquipmentType*)malloc(size*sizeof(EquipmentType));
        for (int i=0; i<size; i++)
            this->slot_equipment_types[i] = container_ui->slot_equipment_types[i];

        int equipment_type_counts[N_EQUIPMENT_TYPES] = {0};
        for (int i=0; i<size; i++)
        {
            EquipmentType type = this->slot_equipment_types[i];
            equipment_type_counts[type] += 1;
            int slot = this->get_render_slot_for_equipment_type(type, equipment_type_counts[type] - 1);
            IF_ASSERT(slot == NULL_SLOT) continue;
            this->render_slot_map[slot] = i;
        }
        UIElement::set_container_id(container_id);
    }

    virtual void init()
    {
        this->xdim = ItemContainer::get_container_xdim(ItemContainer::name::equipment);
        this->ydim = ItemContainer::get_container_ydim(ItemContainer::name::equipment);
        GS_ASSERT(this->xdim * this->ydim <= this->render_dim);
        this->init_item_labels(this->render_dim);
    }

    virtual void draw();
    virtual void draw_name();

    EquipmentUI() :
        xdim(0), ydim(0), slot_equipment_types(NULL)
    {
        this->texture = &EquipmentTexture;
        for (int i=0; i<this->render_dim; i++)
            this->render_slot_map[i] = NULL_SLOT;
    }

    virtual ~EquipmentUI()
    {
        if (this->slot_equipment_types != NULL) free(this->slot_equipment_types);
    }

    protected:

    void draw_background();
    void draw_highlight();
    void draw_items();
    void draw_item_labels();
    void draw_slot_labels();
    void draw_accessories_label();
};


const struct EquipmentUI::Offset EquipmentUI::slot_label_offsets[N_EQUIPMENT_TYPES] = {
    { 0, 0 },   // NULL
    { 5, 12 },  // BODY
    { 5, 12 },  // HEAD
    { 5, 12 },  // FEET
    { 2, 12 },  // HANDS
    { 2, 32 },  // ACCESSORY
};

const struct EquipmentUI::Offset EquipmentUI::slot_label_origins[N_EQUIPMENT_TYPES] = {
    { 0, 0 },      // NULL
    { 136, 9 },    // BODY
    { 136, 0 },    // HEAD
    { 136, 18 },   // FEET
    { 136, 27 },   // HANDS
    { 136, 36  },  // ACCESSORY
};

const struct EquipmentUI::Dim EquipmentUI::slot_label_dimensions[N_EQUIPMENT_TYPES] = {
    { 0, 0 },     // NULL
    { 25, 9 },    // BODY
    { 25, 9 },    // HEAD
    { 25, 9 },    // FEET
    { 31, 9 },    // HANDS
    { 14, 82  },  // ACCESSORY
};

const struct EquipmentUI::Offset EquipmentUI::slot_area_offset = { 13, 18 };   // from the top

const EquipmentType EquipmentUI::render_slot_equipment_types[EquipmentUI::render_dim] = {
    EQUIPMENT_TYPE_ACCESSORY,
    EQUIPMENT_TYPE_HEAD,
    NULL_EQUIPMENT_TYPE,
    EQUIPMENT_TYPE_ACCESSORY,
    EQUIPMENT_TYPE_BODY,
    EQUIPMENT_TYPE_HANDS,
    EQUIPMENT_TYPE_ACCESSORY,
    EQUIPMENT_TYPE_FEET,
    NULL_EQUIPMENT_TYPE
};


void EquipmentUI::draw()
{
    //this->draw_name();
    this->draw_background();
    this->draw_highlight();
    this->draw_slot_labels();
    this->draw_items();
    this->draw_accessories_label();
    this->draw_item_labels();
}


void EquipmentUI::draw_name()
{
    HudFont::start_font_draw();
    HudFont::reset_default();
    HudFont::set_texture();
    float y = this->yoff + this->name.get_height() + 4;
    this->name.set_position(this->xoff, y);
    this->name.draw();
    HudFont::end_font_draw();
}


void EquipmentUI::draw_background()
{
    IF_ASSERT(*this->texture == 0) return;

    glDisable(GL_DEPTH_TEST);
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, *this->texture);
    draw_bound_texture_sprite(
        this->xoff, this->yoff - this->height,
        this->width, this->height,
        -1.0f,
        0.0f, 0.0f,
        float(this->width)/float(this->texture_width),
        float(this->height)/float(this->texture_height));
    glDisable(GL_TEXTURE_2D);
}


void EquipmentUI::draw_highlight()
{
    int hover_slot = this->get_render_slot_at(mouse_x, mouse_y);
    if (hover_slot == NULL_SLOT) return;

    int i = hover_slot % this->render_xdim;
    int j = hover_slot / this->render_xdim;

    float x = this->xoff + (this->slot_highlight_offset + this->slot_area_offset.x + i*slot_size);
    float y = this->yoff - (this->slot_highlight_offset + this->slot_area_offset.y + j*slot_size);

    glColor4ub(160, 160, 160, 128);
    glBegin(GL_QUADS);
    glVertex2f(x,                        y);
    glVertex2f(x + this->highlight_size, y);
    glVertex2f(x + this->highlight_size, y - this->highlight_size);
    glVertex2f(x,                        y - this->highlight_size);
    glEnd();
}


void EquipmentUI::draw_slot_labels()
{
    struct ItemContainer::SlotMetadata* slot_metadata = NULL;
    if (this->container_id != NULL_CONTAINER)
        slot_metadata = ItemContainer::get_container_ui_slot_metadata(this->container_id);

    IF_ASSERT(*this->texture == 0) return;
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, *this->texture);

    int equipment_type_counts[N_EQUIPMENT_TYPES] = {0};
    for (int i=0; i<(int)N_EQUIPMENT_TYPES; i++)
    {
        if ((EquipmentType)i == NULL_EQUIPMENT_TYPE ||
            (EquipmentType)i == EQUIPMENT_TYPE_ACCESSORY) continue;

        equipment_type_counts[i] += 1;   // increment first in case we continue;
        int slot = this->get_render_slot_for_equipment_type((EquipmentType)i, equipment_type_counts[i] - 1);
        IF_ASSERT(slot == NULL_SLOT) continue;
        int xslot = slot % this->render_xdim;
        int yslot = slot / this->render_xdim;

        if (slot_metadata != NULL && this->render_slot_map[slot] != NULL_SLOT &&
            slot_metadata[this->render_slot_map[slot]].type != NULL_ITEM_TYPE) continue;

        int w = this->slot_label_dimensions[i].x;
        int h = this->slot_label_dimensions[i].y;
        int x = xslot*this->slot_size + this->slot_label_offsets[i].x;
        int y = (this->render_ydim - yslot - 1)*this->slot_size + (this->slot_size - this->slot_label_offsets[i].y - h);

        draw_bound_texture_sprite(
            this->xoff + this->slot_area_offset.x + x,
            (this->yoff - this->height) + y + this->slot_area_offset.y - 2,
            w, h,
            -1.0f,
            float(this->slot_label_origins[i].x) / (this->texture_width),
            float(this->slot_label_origins[i].y) / float(this->texture_height),
            float(w) / float(this->texture_width),
            float(h) / float(this->texture_height));
    }

    glDisable(GL_TEXTURE_2D);
}


void EquipmentUI::draw_accessories_label()
{
    const EquipmentType type = EQUIPMENT_TYPE_ACCESSORY;
    IF_ASSERT(*this->texture == 0) return;
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, *this->texture);
    int w = this->slot_label_dimensions[type].x;
    int h = this->slot_label_dimensions[type].y;
    draw_bound_texture_sprite(
        this->xoff + this->slot_label_offsets[type].x,
        this->yoff + this->slot_label_offsets[type].y - this->height,
        w, h,
        -1.0f,
        float(this->slot_label_origins[type].x) / (this->texture_width),
        float(this->slot_label_origins[type].y) / float(this->texture_height),
        float(w) / float(this->texture_width),
        float(h) / float(this->texture_height));

    glDisable(GL_TEXTURE_2D);
}


void EquipmentUI::draw_items()
{
    if (this->container_id == NULL_CONTAINER) return;
    struct ItemContainer::SlotMetadata* slot_metadata = ItemContainer::get_container_ui_slot_metadata(this->container_id);
    if (slot_metadata == NULL) return;

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);
    glBegin(GL_QUADS);

    int equipment_type_counts[N_EQUIPMENT_TYPES] = {0};
    for (int i=0; i<this->xdim*this->ydim; i++)
    {
        EquipmentType eq_type = this->slot_equipment_types[i];
        equipment_type_counts[eq_type] += 1;   // increment first in case we continue;
        int slot = this->get_render_slot_for_equipment_type(eq_type, equipment_type_counts[eq_type] - 1);
        IF_ASSERT(slot == NULL_SLOT) continue;
        if (slot_metadata[i].type == NULL_ITEM_TYPE) continue;

        int tex_id = Item::get_sprite_index_for_type(slot_metadata[i].type);
        int xslot = slot % this->render_xdim;
        int yslot = slot / this->render_xdim;

        float x = this->xoff + (this->slot_item_offset + this->slot_area_offset.x + xslot*slot_size);
        float y = this->yoff - (this->slot_item_offset + this->slot_area_offset.y + yslot*slot_size);

        const float iw = 1.0f/16.0f; // icon_width in texture
        const int iiw = 16; // icon width in pixels

        const float tx_min = iw * (tex_id % iiw);
        const float ty_min = iw * (tex_id / iiw);
        const float tx_max = tx_min + iw;
        const float ty_max = ty_min + iw;

        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);
        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x + this->item_size, y);
        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x + this->item_size, y - this->item_size);
        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x, y - this->item_size);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void EquipmentUI::draw_item_labels()
{
    if (this->container_id == NULL_CONTAINER) return;
    struct ItemContainer::SlotMetadata* slot_metadata = ItemContainer::get_container_ui_slot_metadata(this->container_id);
    if (slot_metadata == NULL) return;

    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    int equipment_type_counts[N_EQUIPMENT_TYPES] = {0};
    for (int i=0; i<this->xdim*this->ydim; i++)
    {
        EquipmentType eq_type = this->slot_equipment_types[i];
        equipment_type_counts[eq_type] += 1;   // increment first in case we continue;
        int slot = this->get_render_slot_for_equipment_type(eq_type, equipment_type_counts[eq_type] - 1);
        IF_ASSERT(slot == NULL_SLOT) continue;
        if (slot_metadata[i].type == NULL_ITEM_TYPE) continue;

        int stack = slot_metadata[i].stack_size;
        int charges = slot_metadata[i].charges;
        HudText::Text* text = &this->item_labels[slot];

        int xslot = slot % this->render_xdim;
        int yslot = slot / this->render_xdim;
        const float x = this->xoff + ((xslot+1)*this->slot_size - this->slot_highlight_offset + this->slot_area_offset.x - text->get_width());
        const float y = this->yoff - ((yslot+1)*this->slot_size - this->slot_highlight_offset + this->slot_area_offset.y - text->get_height());
        draw_slot_numbers(text, x, y + 1, stack, charges);
    }
    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST); // move render somewhere
    glDisable(GL_BLEND);
}

}   // HudContainer
