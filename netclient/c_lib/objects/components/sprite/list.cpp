#include "list.hpp"

#if DC_CLIENT

#include <c_lib/draw/draw.hpp>

void SpriteList::draw()
{
    if (this->ct == 0) return;
    SpriteProperties* data;
    for (int i=0; i<this->max; i++)
        if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
        {
            data = (SpriteProperties*)this->objects[i];
            drawBillboardSprite(data->obj->get_position(), data->sprite_index, data->scale);
        }
}

#endif
