#include "list.hpp"

#if DC_CLIENT
#include <c_lib/objects/components/minivox/colored_minivox.hpp>
#include <c_lib/objects/components/minivox/textured_minivox.hpp>
#include <c_lib/draw/draw.hpp>

void ColoredMinivoxList::draw()
{
    if (this->ct == 0) return;
    ColoredMinivoxProperties* data;
    for (int i=0; i<this->max; i++)
        if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
        {
            data = (ColoredMinivoxProperties*)this->objects[i];
            drawColoredMinivox(
                data->obj->get_position(),
                data->forward, data->right, data->normal,
                data->color
            );
        }
}

void TexturedMinivoxList::draw()
{
    if (this->ct == 0) return;
    TexturedMinivoxProperties* data;
    for (int i=0; i<this->max; i++)
        if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
        {
            data = (TexturedMinivoxProperties*)this->objects[i];
            drawTexturedMinivox(
                data->obj->get_position(),
                data->forward, data->right, data->normal,
                data->texture_index,
                data->tx, data->ty, data->pixel_margin
            );
        }
}

#endif
