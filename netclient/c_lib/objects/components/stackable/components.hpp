#pragma once

// TODO -- have to share stack counts with rendering info

class StackableProperties
{
    public:
        int max;
        int count;

    StackableProperties()
    : max(1), count(1)
    {}
};

class StackableComponent
{
    public:

        StackableProperties properties;

        StackableComponent(){}
};
