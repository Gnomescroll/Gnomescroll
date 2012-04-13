#include "list.hpp"

#if DC_CLIENT

#include <c_lib/draw/draw.hpp>

void ColoredMinivoxList::draw()
{
    if (this->ct == 0) return;
    MinivoxProperties* data;
    for (int i=0; i<this->max; i++)
        if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
        {
            data = (MinivoxProperties*)this->objects[i];
            drawMinivox(
                data->obj->get_position(),
                data->forward, data->right, data->normal,
                data->size, data->color
            );
        }
}

#endif
