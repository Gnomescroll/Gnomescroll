#include "_interface.hpp"

#include <entity/constants.hpp>
#include <hud/container/constants.hpp>
#include <hud/container/inventory.hpp>
#include <hud/container/toolbelt.hpp>
#include <hud/container/synthesizer.hpp>
#include <hud/container/storage_block.hpp>
#include <hud/container/crusher.hpp>
#include <hud/container/cache.hpp>

namespace HudContainer
{

static bool agent_inventory_enabled = false;
static bool container_block_enabled = false;
static ItemContainerID container_block_enabled_id = NULL_CONTAINER;
float mouse_x = -1;
float mouse_y = -1;
bool lm_down = false;

// private containers
class AgentInventoryUI* agent_inventory = NULL;
class AgentToolbeltUI* agent_toolbelt = NULL;
class AgentSynthesizerUI* synthesizer_container = NULL;
class EnergyTanksUI* energy_tanks = NULL;
class CacheUI* premium_cache = NULL;
class EquipmentUI* equipment = NULL;

// public containers
class CraftingUI* crafting_container = NULL;
class StorageBlockUI* storage_block = NULL;
class SmelterUI* smelter = NULL;
class CrusherUI* crusher = NULL;

// array holding all those containers for convenient lookup
UIElement** ui_elements = NULL;

UIElement* get_ui_element(ItemContainerType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    return ui_elements[type];
}

void set_container_id(ItemContainerType container_type, ItemContainerID container_id)
{
    if (container_type == ItemContainer::name::hand) return;
    UIElement* container_ui = get_ui_element(container_type);
    IF_ASSERT(container_ui == NULL) return;
    container_ui->set_container_id(container_id);
    container_ui->set_container_type(container_type);
    container_ui->name.set_text(ItemContainer::get_container_pretty_name(container_type));
}

void close_container(ItemContainerID container_id)
{
    // unset ids for free-world container UIs
    bool closed = false;
    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        if (ui_elements[i] == NULL) continue;
        if (!ItemContainer::container_type_is_attached_to_agent((ItemContainerType)i)) continue;
        if (ui_elements[i]->container_id == container_id)
        {
            // should not have 2 ui_elements with same container id active,
            // but we are closing them all just in case
            GS_ASSERT(!closed);
            closed = true;
            ui_elements[i]->container_id = NULL_CONTAINER;
        }
    }

    if (container_block_enabled && closed)
        disable_container_block_hud();
}

// Input Handling

void enable_agent_inventory_hud()
{
    GS_ASSERT(!container_block_enabled);
    agent_inventory_enabled = true;
}

void disable_agent_inventory_hud()
{
    // reset mouse state
    mouse_x = -1;
    mouse_y = -1;
    agent_inventory_enabled = false;
}

void enable_container_block_hud(ItemContainerID container_id)
{
    GS_ASSERT(!agent_inventory_enabled);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(container_block_enabled_id == NULL_CONTAINER);
    container_block_enabled = true;
    container_block_enabled_id = container_id;
}

void disable_container_block_hud()
{
    mouse_x = -1;
    mouse_y = -1;
    container_block_enabled = false;
    container_block_enabled_id = NULL_CONTAINER;
}

static UIElement* get_container_and_slot(int x, int y, int* slot)
{
    // track topmost clicked container
    UIElement* closest_container = NULL;
    int closest_slot = NULL_SLOT;

    // get topmost container click
    // WARNING: doesnt support overlapping containers yet.
    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        UIElement* container = ui_elements[i];
        if (container == NULL) continue;
        if (container->container_id == NULL_CONTAINER) continue;
        if (!container->point_inside(x,y)) continue;
        closest_container = container;
        closest_slot = container->get_slot_at(x,y);
    }

    *slot = closest_slot;
    return closest_container;
}


static ContainerInputEvent get_container_hud_ui_event(int x, int y)
{
    // detect click
    int slot;
    UIElement* container = get_container_and_slot(x,y, &slot);

    ItemContainerID container_id = NULL_CONTAINER;
    if (container != NULL) container_id = container->container_id;

    ContainerInputEvent event;
    event.container_id = container_id;
    event.slot = slot;
    event.alt_action = (container != NULL &&
                       ((container->type == UI_ELEMENT_SYNTHESIZER_CONTAINER &&
                       ((AgentSynthesizerUI*)container)->in_shopping_region(x,y)) ||
                       (container->type == UI_ELEMENT_CRAFTING_CONTAINER &&
                       ((CraftingUI*)container)->in_craft_output_region(x,y)) ||
                       (container->type == UI_ELEMENT_CRUSHER &&
                       ((CrusherUI*)container)->in_button_region(x,y))));
    return event;
}

// returns item type at screen position x,y
static ItemType get_item_type_at(int x, int y)
{
    int slot;
    UIElement* ui = get_container_and_slot(x,y, &slot);
    if (ui == NULL) return NULL_ITEM_TYPE;
    if (ui->container_id == NULL_CONTAINER) return NULL_ITEM_TYPE;
    if (slot == NULL_SLOT) return NULL_ITEM_TYPE;

    using ItemContainer::ItemContainerUIInterface;
    ItemContainerUIInterface* container = ItemContainer::get_container_ui(ui->container_id);
    if (container == NULL) return NULL_ITEM_TYPE;

    if (ui->type == UI_ELEMENT_SYNTHESIZER_CONTAINER)
    {
        using ItemContainer::ItemContainerSynthesizerUI;
        if (((AgentSynthesizerUI*)ui)->in_shopping_region(x,y))
        {
            int xslot = slot % ((AgentSynthesizerUI*)ui)->shopping_xdim;
            int yslot = slot / ((AgentSynthesizerUI*)ui)->shopping_xdim;
            return Item::get_synthesizer_item(xslot, yslot);
        }
        else if (((AgentSynthesizerUI*)ui)->in_coins_region(x,y))
            return ((ItemContainerSynthesizerUI*)container)->get_coin_type();
        return NULL_ITEM_TYPE;
    }

    if (ui->type == UI_ELEMENT_CRAFTING_CONTAINER)
    {
        if (((CraftingUI*)ui)->in_craft_output_region(x,y))
            return Item::get_selected_craft_recipe_type(container->id, slot);
    }

    return container->get_slot_type(slot);
}

static const ContainerInputEvent NULL_EVENT = {
    NULL_CONTAINER,         // null container id
    NULL_SLOT,              // null slot
    false,                  // alt action
};

ContainerInputEvent left_mouse_down(int x, int y)
{
    set_mouse_position(x, y);
    lm_down = true;
    return NULL_EVENT;
}

ContainerInputEvent left_mouse_up(int x, int y)
{
    set_mouse_position(x, y);
    lm_down = false;
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent right_mouse_down(int x, int y)
{
    set_mouse_position(x, y);
    return NULL_EVENT;
}

ContainerInputEvent right_mouse_up(int x, int y)
{
    set_mouse_position(x, y);
    return get_container_hud_ui_event(x,y);
}

ContainerInputEvent mouse_motion(int x, int y)
{
    set_mouse_position(x, y);
    return NULL_EVENT;
}

void set_mouse_position(int x, int y)
{
    mouse_x = x;
    mouse_y = y;
}

ContainerInputEvent scroll_up()
{
    if (agent_toolbelt == NULL) return NULL_EVENT;
    agent_toolbelt->selected_slot -= 1;
    agent_toolbelt->selected_slot %= agent_toolbelt->xdim;
    if (agent_toolbelt->selected_slot < 0) agent_toolbelt->selected_slot += agent_toolbelt->xdim;

    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = agent_toolbelt->selected_slot;
    event.alt_action = false;
    return event;
}

ContainerInputEvent scroll_down()
{
    if (agent_toolbelt == NULL) return NULL_EVENT;
    agent_toolbelt->selected_slot += 1;
    agent_toolbelt->selected_slot %= agent_toolbelt->xdim;

    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = agent_toolbelt->selected_slot;
    event.alt_action = false;
    return event;
}

ContainerInputEvent select_slot(int numkey)
{
    if (agent_toolbelt == NULL) return NULL_EVENT;

    if (numkey == 0) numkey = 10;
    int slot = numkey-1;
    if (slot < 0 || slot >= agent_toolbelt->xdim) return NULL_EVENT;
    agent_toolbelt->selected_slot = slot;

    ContainerInputEvent event;
    event.container_id = agent_toolbelt->container_id;
    event.slot = slot;
    event.alt_action = false;
    return event;
}

// Drawing

static HudText::Text* grabbed_icon_stack_text = NULL;
static HudText::Text* tooltip_text = NULL;

static void draw_grabbed_icon()
{
    IF_ASSERT(TextureSheetLoader::item_texture_sheet_loader->texture == 0) return;
    using ItemContainer::player_hand_ui;
    if (player_hand_ui == NULL) return;
    struct ItemContainer::SlotMetadata hand_metadata = player_hand_ui->get_item_metadata();
    if (hand_metadata.type == NULL_ITEM_TYPE) return;

    const float w = ITEM_ICON_RENDER_SIZE;

    // center icon on mouse position
    float x = mouse_x - (w / 2);
    float y = _yresf - (mouse_y + (w / 2));

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render durability
    const unsigned char alpha = 128;
    draw_durability_meter(x, y, w, alpha, hand_metadata);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    int tex_id = Item::get_sprite_index_for_type(hand_metadata.type);

    const float iw = 16.0f; // icon_width
    const int iiw = 16; // integer icon width

    const float tx_min = (1.0f/iw)*(tex_id % iiw);
    const float ty_min = (1.0f/iw)*(tex_id / iiw);
    const float tx_max = tx_min + 1.0f/iw;
    const float ty_max = ty_min + 1.0f/iw;

    glTexCoord2f(tx_min, ty_min);
    glVertex2f(x,y+w);

    glTexCoord2f(tx_max, ty_min);
    glVertex2f(x+w, y+w);

    glTexCoord2f(tx_max, ty_max);
    glVertex2f(x+w, y);

    glTexCoord2f(tx_min, ty_max);
    glVertex2f(x, y);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Draw stack numbers
    IF_ASSERT(grabbed_icon_stack_text == NULL) return;
    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();
    x = x + (w/2) + font_size - 7;
    y = y + (w/2) - font_size - 2;
    draw_slot_numbers(grabbed_icon_stack_text, x, y, hand_metadata.stack_size, hand_metadata.charges);
    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

static void draw_tooltip()
{
    using ItemContainer::player_hand_ui;
    // dont draw tooltips if we're holding something
    if (player_hand_ui == NULL || player_hand_ui->get_item_type() != NULL_ITEM_TYPE) return;

    // get item type hovered
    ItemType item_type = get_item_type_at(mouse_x, mouse_y);
    if (item_type == NULL_ITEM_TYPE) return;

    // get name
    const char* name = Item::get_item_pretty_name(item_type);
    IF_ASSERT(name == NULL) return;
    IF_ASSERT(name[0] == '\0') return;

    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    // align
    tooltip_text->set_position(mouse_x + 15, _yresf-mouse_y);

    tooltip_text->set_text(name);
    tooltip_text->draw();

    HudFont::reset_default();
    HudFont::end_font_draw();
}

void draw()
{
    agent_toolbelt->draw();
    energy_tanks->inv_open = agent_inventory_enabled;
    energy_tanks->draw();

    if (!agent_inventory_enabled && !container_block_enabled) return;

    energy_tanks->draw_name();
    agent_toolbelt->draw_name();
    agent_inventory->draw();
    synthesizer_container->draw();
    premium_cache->draw();
    equipment->draw();

    if (container_block_enabled)
    {
        GS_ASSERT(container_block_enabled_id != NULL_CONTAINER);
        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_block_enabled_id);
        GS_ASSERT(container != NULL);
        if (container != NULL)
        {
            GS_ASSERT(container->type != NULL_CONTAINER_TYPE);
            if (isValid(container->type))
            {
                GS_ASSERT(ui_elements[container->type] != NULL);
                if (ui_elements[container->type] != NULL)
                {
                    ui_elements[container->type]->set_container_type(container->type);
                    ui_elements[container->type]->draw();
                }
            }
        }
    }

    draw_grabbed_icon();
    draw_tooltip();
}

/* Main init/teardown */

void init()
{
    synthesizer_container = new AgentSynthesizerUI;
    synthesizer_container->type = UI_ELEMENT_SYNTHESIZER_CONTAINER;
    synthesizer_container->init();
    synthesizer_container->xoff = (_xresf - synthesizer_container->width())/2;
    synthesizer_container->yoff = 120.0f + (_yresf + synthesizer_container->height())/2;

    agent_inventory = new AgentInventoryUI;
    agent_inventory->type = UI_ELEMENT_AGENT_INVENTORY;
    agent_inventory->init();
    agent_inventory->xoff = (_xresf - agent_inventory->width())/2 + 1;  // +1 because the width is odd with odd valued inc1 and even valued xdim
    agent_inventory->yoff = _yresf - (synthesizer_container->yoff - synthesizer_container->height() - 18);

    agent_toolbelt = new AgentToolbeltUI;
    agent_toolbelt->type = UI_ELEMENT_AGENT_TOOLBELT;
    agent_toolbelt->init();
    agent_toolbelt->xoff = (_xresf - agent_toolbelt->width())/2;
    agent_toolbelt->yoff = _yresf - (agent_toolbelt->height());

    energy_tanks = new EnergyTanksUI;
    energy_tanks->type = UI_ELEMENT_ENERGY_TANKS;
    energy_tanks->init();
    energy_tanks->xoff = ((_xresf - agent_toolbelt->width())/2);
    energy_tanks->yoff = 0;

    crafting_container = new CraftingUI;
    crafting_container->type = UI_ELEMENT_CRAFTING_CONTAINER;
    crafting_container->init();
    crafting_container->xoff = agent_inventory->xoff + agent_inventory->width() + 6;
    crafting_container->yoff = _yresf - agent_inventory->yoff - 2;

    storage_block = new StorageBlockUI;
    storage_block->type = UI_ELEMENT_STORAGE_BLOCK;
    storage_block->set_container_type(ItemContainer::name::storage_block_small);
    storage_block->init();
    storage_block->centered = true;
    storage_block->xoff = agent_inventory->xoff + agent_inventory->width() + 6;
    storage_block->yoff = _yresf - agent_inventory->yoff - 2;

    smelter = new SmelterUI;
    smelter->type = UI_ELEMENT_SMELTER;
    smelter->set_container_type(ItemContainer::name::smelter_basic);
    smelter->init();
    smelter->centered = true;
    smelter->xoff = agent_inventory->xoff + agent_inventory->width() + 6;
    smelter->yoff = _yresf - agent_inventory->yoff - 2;

    crusher = new CrusherUI;
    crusher->type = UI_ELEMENT_CRUSHER;
    crusher->set_container_type(ItemContainer::name::crusher);
    crusher->init();
    crusher->centered = true;
    crusher->xoff = agent_inventory->xoff + agent_inventory->width() + 6;
    crusher->yoff = _yresf - agent_inventory->yoff - 2;

    premium_cache = new CacheUI;
    premium_cache->type = UI_ELEMENT_CACHE;
    premium_cache->set_container_type(ItemContainer::name::premium_cache);
    premium_cache->init();
    premium_cache->xoff = agent_inventory->xoff - (premium_cache->width() + 8);
    premium_cache->yoff = agent_inventory->yoff;

    equipment = new EquipmentUI;
    equipment->type = UI_ELEMENT_EQUIPMENT;
    equipment->set_container_type(ItemContainer::name::equipment);
    equipment->init();
    // coordinates are point to top left on screen, where y=0 is along the bottom
    equipment->xoff = agent_inventory->xoff - (equipment->width + 6);
    equipment->yoff = synthesizer_container->yoff;

    grabbed_icon_stack_text = new HudText::Text;
    grabbed_icon_stack_text->set_format("%d");
    grabbed_icon_stack_text->set_format_extra_length(11 + 1 - 2);
    grabbed_icon_stack_text->set_color(Color(255,255,255,255));
    grabbed_icon_stack_text->set_depth(-0.1f);

    tooltip_text = new HudText::Text;
    tooltip_text->set_color(Color(255,255,255,255));
    tooltip_text->set_depth(-0.1f);

    // pack ui elements into the array
    GS_ASSERT(ui_elements == NULL);
    ui_elements = (UIElement**)calloc(MAX_CONTAINER_TYPES, sizeof(UIElement*));

    ui_elements[ItemContainer::name::inventory] = agent_inventory;
    ui_elements[ItemContainer::name::toolbelt] = agent_toolbelt;
    ui_elements[ItemContainer::name::energy_tanks] = energy_tanks;
    ui_elements[ItemContainer::name::premium_cache] = premium_cache;
    ui_elements[ItemContainer::name::synthesizer] = synthesizer_container;
    ui_elements[ItemContainer::name::crafting_bench_basic] = crafting_container;
    ui_elements[ItemContainer::name::storage_block_small] = storage_block;
    ui_elements[ItemContainer::name::cryofreezer_small] = storage_block;    // both use storage block instance
    ui_elements[ItemContainer::name::smelter_basic] = smelter;
    ui_elements[ItemContainer::name::equipment] = equipment;
    ui_elements[ItemContainer::name::crusher] = crusher;
}

void teardown()
{
    if (agent_inventory != NULL) delete agent_inventory;
    if (agent_toolbelt != NULL) delete agent_toolbelt;
    if (synthesizer_container != NULL) delete synthesizer_container;
    if (crafting_container != NULL) delete crafting_container;
    if (storage_block != NULL) delete storage_block;
    if (smelter != NULL) delete smelter;
    if (energy_tanks != NULL) delete energy_tanks;
    if (crusher != NULL) delete crusher;
    if (premium_cache != NULL) delete premium_cache;
    if (equipment != NULL) delete equipment;
    if (grabbed_icon_stack_text != NULL) delete grabbed_icon_stack_text;
    if (ui_elements != NULL) free(ui_elements);
}

void draw_durability_meter(float x, float y, int slot_size, unsigned char alpha,
                           const struct ItemContainer::SlotMetadata& metadata)
{
    int durability = metadata.durability;
    if (durability == NULL_DURABILITY) return;
    int max_durability = Item::get_max_durability(metadata.type);
    float ratio = float(durability)/float(max_durability);
    int mh = slot_size / 8; // meter height
    glColor4ub(255, 0, 0, alpha);               // red
    Hud::meter_graphic.draw(x, y, slot_size, mh, 1.0f); // full slot width background
    Hud::set_color_from_ratio(ratio, alpha);
    Hud::meter_graphic.draw(x, y, slot_size, mh, ratio);
}

void draw_init()
{
    init_texture();
}

void draw_teardown()
{
    teardown_texture();
}

void draw_tracking_pixel(float x, float y)
{
    // Draw dot in upper left corner
    GL_ASSERT(GL_TEXTURE_2D, false);

    glColor4ub(255, 0, 0, 255);

    float p = 1.0f;
    glBegin(GL_QUADS);

    glVertex2f(x-p, y+p);
    glVertex2f(x+p, y+p);
    glVertex2f(x+p, y-p);
    glVertex2f(x-p, y-p);

    glEnd();
}

}   // HudContainer
