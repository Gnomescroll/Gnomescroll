#include "lists.hpp"

/* List management */

void BehaviourList::register_object(ListProperties* state)
{
    if (this->ct >= this->max)
    {
        printf("WARNING: %s is full\n", name());
        return;
    }
    int i=0;
    for (;i<this->max; i++)
    {
        if (this->objects[i] == NULL)
        {
            state->id = i;
            state->list = this;
            this->objects[i] = state;
            this->ct++;
            break;
        }
    }
    if (i == this->max)
        printf("WARNING: no empty slots found in %s\n", name());
}

void BehaviourList::unregister_object(ListProperties* state)
{
    if (state->list == NULL || state->id < 0 || state->id >= this->max)
        return;
    if (this->objects[state->id] == NULL) return;
    this->objects[state->id] = NULL;
    this->ct--;
    state->list = NULL;
    state->id = -1;
    return;
}

