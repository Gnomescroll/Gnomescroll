#include "status.hpp"

#include <hud/container/_interface.hpp>
#include <agent/attributes.hpp>

namespace HudStatus
{

struct SpriteIndex
{
    int x, y;
};

static const int ICON_SIZE = 16;
static const int TEX_WIDTH = 128;
static const int TEX_HEIGHT = 128;
static const int ICONS_WIDE = TEX_WIDTH / ICON_SIZE;
static const int ICONS_HIGH = TEX_HEIGHT / ICON_SIZE;
static const float ICON_WIDTH = 1.0f / float(ICONS_WIDE);
static const float ICON_HEIGHT = 1.0f / float(ICONS_HIGH);

static const float toolbelt_margin = -4.0f;
static const int health_icons_wide = 10;

static const struct SpriteIndex hunger_full = { 0, 7 };
static const struct SpriteIndex hunger_empty =  { 1, 7 };

static GLuint texture = 0;

void init()
{
    GS_ASSERT(texture == 0);
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file(MEDIA_PATH "sprites/icons/status.png",
                                       &texture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(texture != 0);
}

void draw_health()
{
    using HudContainer::toolbelt;
    if (toolbelt == NULL) return;
    AgentID agent_id = ClientState::player_agent.agent_id;
    if (!isValid(agent_id)) return;

    int health = Agents::get_attribute_int(agent_id, "health");
    int health_max = Agents::get_attribute_int(agent_id, "health_max");
    int last_health_max = health_max % 4;
    int last_health = health % 4;

    float x = toolbelt->xoff;
    float ox = x;
    float y = _yresf - (toolbelt->yoff - toolbelt_margin);

    int pos = 1;
    for (int i=1; i<=health/4; i++, pos++)
    {   // draw full health
        float sx = 0.0f;
        float sy = 0.0f;
        draw_bound_texture_sprite(x, y, ICON_SIZE, ICON_SIZE, -1.0f, sx, sy,
                                  ICON_WIDTH, ICON_HEIGHT);
        x += ICON_SIZE;
        if (pos && pos % health_icons_wide == 0)
        {
            x = ox;
            y += ICON_SIZE;
        }
    }

    int i = health/4;
    if (last_health)
    {   // partially filled
        i += 1;
        float sx = (4 - last_health) * ICON_WIDTH;
        float sy = 0.0f;
        if (last_health_max && 4 > health_max - health)
        {   // partial, and partially filled
            sy = (4 - last_health_max) * ICON_HEIGHT;
        }
        draw_bound_texture_sprite(x, y, ICON_SIZE, ICON_SIZE, -1.0f, sx, sy,
                                  ICON_WIDTH, ICON_HEIGHT);
        x += ICON_SIZE;
        if (pos && pos % health_icons_wide == 0)
        {
            x = ox;
            y += ICON_SIZE;
        }
        pos++;
    }

    for (; i<health_max/4; i++, pos++)
    {   // draw empties
        float sx = 4 * ICON_WIDTH;
        float sy = 0.0f;
        draw_bound_texture_sprite(x, y, ICON_SIZE, ICON_SIZE, -1.0f, sx, sy,
                                  ICON_WIDTH, ICON_HEIGHT);
        x += ICON_SIZE;
        if (pos && pos % health_icons_wide == 0)
        {
            x = ox;
            y += ICON_SIZE;
        }
    }

    if (last_health_max && last_health_max < health_max - health)
    {   // draw last empty partial icon
        float sx = 4 * ICON_WIDTH;
        float sy = (4 - last_health_max) * ICON_HEIGHT;
        draw_bound_texture_sprite(x, y, ICON_SIZE, ICON_SIZE, -1.0f, sx, sy,
                                  ICON_WIDTH, ICON_HEIGHT);
    }
}

void draw_hunger()
{
    using HudContainer::toolbelt;
    if (toolbelt == NULL) return;
    AgentID agent_id = ClientState::player_agent.agent_id;
    if (!isValid(agent_id)) return;

    int hunger = Agents::get_attribute_int(agent_id, "hunger");
    int hunger_max = Agents::get_attribute_int(agent_id, "hunger_max");
    int fullness = hunger_max - hunger;

    int width = ICON_SIZE * hunger_max;
    float x = toolbelt->xoff + toolbelt->width() - width - 4;
    float y = _yresf - (toolbelt->yoff - toolbelt_margin);

    float full_sx = hunger_full.x * ICON_WIDTH;
    float full_sy = hunger_full.y * ICON_HEIGHT;
    float empty_sx = hunger_empty.x * ICON_WIDTH;
    float empty_sy = hunger_empty.y * ICON_HEIGHT;

    static const Color red = Color(198, 57, 57);

    const int tick_rate = (ONE_SECOND/2) / (GS_MAX(0, hunger - HUNGER_DAMAGE_THRESHOLD ) + 1);
    static int dir = 1;
    static int rate = 0;
    if (rate <= 0)
    {
        rate = GS_MAX(-1, rate);
        dir = 1;
    }
    else if (rate >= tick_rate)
    {
        rate = tick_rate;
        dir = -1;
    }
    rate += dir;

    if (hunger >= HUNGER_DAMAGE_THRESHOLD)
    {   // make it blink
        float f = float(rate) / float(tick_rate);
        Color c = interpolate_color(COLOR_WHITE, red, f);
        glColor4ub(c.r, c.g, c.b, 255);
    }

    for (int i=1; i<=hunger_max; i++)
    {
        float sx = 0.0f;
        float sy = 0.0f;
        if (i <= fullness)
        {
            sx = full_sx;
            sy = full_sy;
        }
        else
        {
            sx = empty_sx;
            sy = empty_sy;
        }
        draw_bound_texture_sprite(x, y, ICON_SIZE, ICON_SIZE, -1.0f, sx, sy,
                                  ICON_WIDTH, ICON_HEIGHT);
        x += ICON_SIZE;
    }

    glColor4ub(255, 255, 255, 255);
}

void draw()
{
    IF_ASSERT(texture == 0) return;
    GL_ASSERT(GL_TEXTURE_2D, true);
    glBindTexture(GL_TEXTURE_2D, texture);

    draw_health();
    draw_hunger();
}


}   // HudStatus
