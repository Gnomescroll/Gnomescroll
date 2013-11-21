/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/template/object_list.hpp>

typedef enum
{
    NULL_MODIFIER = 0xFF
}   ModifierID;

typedef enum
{
    NULL_MODIFIER_CHANGE = 0,
    MODIFIER_CHANGE_AMOUNT,
    MODIFIER_CHANGE_PERCENT,
}   ModifierChangeType;

typedef enum
{
    NULL_MODIFIER_EVENT = 0,
    MODIFIER_EVENT_CONSTANT,
    MODIFIER_EVENT_INSTANT,
    MODIFIER_EVENT_DURATION,
    MODIFIER_EVENT_PERIODIC,
}   ModifierEventType;

class Modifier
{
    public:
        ModifierID id;
        AttributeType attribute_type;
        ModifierChangeType change_type;
        ModifierEventType event_type;

        int amount;
        float percent;

        int duration;
        int period;

    void copy_from(const Modifier* other)
    {
        this->attribute_type = other->attribute_type;
        this->change_type = other->change_type;
        this->event_type = other->event_type;
        this->duration = other->duration;
        this->period = other->period;
        this->amount = other->amount;
        this->percent = other->percent;
    }

    void set_amount(int amount)
    {
        GS_ASSERT(this->change_type == NULL_MODIFIER_CHANGE);
        this->change_type = MODIFIER_CHANGE_AMOUNT;
        this->amount = amount;
    }

    void set_percent(float percent)
    {
        GS_ASSERT(this->change_type == NULL_MODIFIER_CHANGE);
        this->change_type = MODIFIER_CHANGE_PERCENT;
        this->percent = percent/100.0f;
    }

    void set_event_type(ModifierEventType event_type)
    {
        GS_ASSERT(event_type != NULL_MODIFIER_EVENT);
        this->event_type = event_type;
    }

    void set_periodic(int duration, int period)
    {
        GS_ASSERT(this->duration == -1 && this->period == -1);
        GS_ASSERT(duration >= 0 && period >= 0);
        GS_ASSERT(duration >= period);
        this->duration = duration;
        this->period = period;
        this->event_type = MODIFIER_EVENT_PERIODIC;
    }

    void set_duration(int duration)
    {
        GS_ASSERT(this->duration == -1);
        this->duration = duration;
        this->event_type = MODIFIER_EVENT_DURATION;
    }

    void set_instant()
    {
        this->event_type = MODIFIER_EVENT_INSTANT;
    }

    void set_attribute_type(AttributeType attribute_type)
    {
        GS_ASSERT(this->attribute_type == NULL_ATTRIBUTE);
        this->attribute_type = attribute_type;
    }

    Modifier(ModifierID id) :
        id(id), attribute_type(NULL_ATTRIBUTE),
        change_type(NULL_MODIFIER_CHANGE), event_type(MODIFIER_EVENT_CONSTANT),
        amount(0), percent(0.0f), duration(-1), period(-1)
    {
    }

    void print() const
    {
        printf("Modifier: ");
        printf("Amount: %d, ", this->amount);
        printf("Percent: %0.2f", this->percent);
        printf("\n");
    }

    void verify() const
    {
        GS_ASSERT(this->attribute_type != NULL_ATTRIBUTE);
        GS_ASSERT(this->change_type != NULL_MODIFIER_CHANGE);
        GS_ASSERT(this->change_type != MODIFIER_CHANGE_AMOUNT || this->amount != 0);
        GS_ASSERT(this->change_type != MODIFIER_CHANGE_PERCENT || this->percent != 0.0f);
        GS_ASSERT((this->amount != 0 && this->percent == 0.0f) ||
                  (this->amount == 0 && this->percent != 0.0f));
        GS_ASSERT(this->event_type != NULL_MODIFIER_EVENT);
        if (this->event_type == MODIFIER_EVENT_INSTANT ||
            this->event_type == MODIFIER_EVENT_CONSTANT)
        {
            GS_ASSERT(this->duration == -1 && this->period == -1);
        }
        if (this->event_type == MODIFIER_EVENT_DURATION)
        {
            GS_ASSERT(this->duration > 0 && this->period == -1);
        }
        if (this->event_type == MODIFIER_EVENT_PERIODIC)
        {
            GS_ASSERT(this->duration > 0 && this->period > 0);
            GS_ASSERT(this->duration >= this->period);
        }
    }

    void verify_other(const Modifier* other) const
    {
        GS_ASSERT(this->attribute_type != other->attribute_type);
    }
};


class ModifierList: public ObjectList<Modifier, ModifierID>
{
    public:
    const char* name()
    {
        return "ModifierList";
    }

    Modifier* create()
    {
        return ObjectList<Modifier, ModifierID>::create();
    }

    Modifier* create(ModifierID id)
    {
        GS_ASSERT(false);
        return NULL;
    }

    ModifierList(size_t max) :
        ObjectList<Modifier, ModifierID>(max, NULL_MODIFIER)
    {}
};


