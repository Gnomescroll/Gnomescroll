#pragma once

const int COLORED_MINIVOX_LIST_MAX = 1024;

class ColoredMinivoxList: public BehaviourList
{
    private:
        const char* name() { return "ColoredMinivoxList"; }
    public:
        void draw()
        {
            if (this->ct == 0) return;
            for (int i=0; i<this->max; i++)
                if (this->objects[i] != NULL && this->objects[i]->obj != NULL)
                    this->objects[i]->obj->draw();
        }

    ColoredMinivoxList()
    : BehaviourList(COLORED_MINIVOX_LIST_MAX)
    {}
};
