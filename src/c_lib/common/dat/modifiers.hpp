#pragma once

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
    MODIFIER_EVENT_PERIODIC,
}   ModifierEventType;

class Modifier
{
    public:
        AttributeType attribute_type;
        ModifierChangeType change_type;
        ModifierEventType event_type;

        union
        {
            int ival;
            float fval;
        };

        int duration;
        int period;

    int get_int()
    {
        GS_ASSERT(this->change_type == MODIFIER_CHANGE_AMOUNT)
        return this->ival;
    }

    float get_float()
    {
        GS_ASSERT(this->change_type == MODIFIER_CHANGE_PERCENT);
        return this->fval;
    }

    void set_value(int value)
    {
        GS_ASSERT(this->change_type == NULL_MODIFIER_CHANGE);
        this->change_type = MODIFIER_CHANGE_AMOUNT;
        this->ival = value;
    }

    void set_value(float value)
    {
        GS_ASSERT(this->change_type == NULL_MODIFIER_CHANGE);
        this->change_type = MODIFIER_CHANGE_PERCENT;
        this->fval = value;
    }

    void set_event_type(ModifierEventType event_type)
    {
        GS_ASSERT(this->event_type == NULL_MODIFIER_EVENT);
        GS_ASSERT(event_type != NULL_MODIFIER_EVENT);
        this->event_type = event_type;
    }

    void set_periodic(int duration, int period)
    {
        GS_ASSERT(this->event_type != MODIFIER_EVENT_PERIODIC);
        GS_ASSERT(this->duration != 0 && this->period != 0);
        this->duration = duration;
        this->period = period;
        this->event_type = MODIFIER_EVENT_PERIODIC;
    }

    Modifier() :
        attribute_type(NULL_ATTRIBUTE),
        change_type(NULL_MODIFIER_CHANGE), event_type(MODIFIER_EVENT_CONSTANT),
        ival(0), duration(0), period(0)
    {
    }

    void verify() const
    {
        GS_ASSERT(this->attribute_type != NULL_ATTRIBUTE);
        GS_ASSERT(this->change_type != NULL_MODIFIER_CHANGE);
        GS_ASSERT(this->change_type != MODIFIER_CHANGE_AMOUNT || this->ival != 0);
        GS_ASSERT(this->change_type != MODIFIER_CHANGE_PERCENT || this->fval != 1.0f);
        GS_ASSERT(this->event_type != NULL_MODIFIER_EVENT);
        GS_ASSERT(this->event_type != MODIFIER_EVENT_PERIODIC ||
                  (this->duration != 0 && this->period != 0));
        GS_ASSERT(this->event_type != MODIFIER_EVENT_CONSTANT ||
                  (this->duration == 0 && this->period == 0));
    }

    void verify_other(const Modifier* other) const
    {
        GS_ASSERT(this->attribute_type != other->attribute_type);
    }
};
