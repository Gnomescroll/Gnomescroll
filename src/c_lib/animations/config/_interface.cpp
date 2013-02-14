#include "_interface.hpp"

#include <animations/config/_state.hpp>

#if DC_CLIENT
# include <animations/animations.hpp>
#endif

namespace Animations
{

void dummy(AnimationType type, void* metadata) {}

#if DC_SERVER
// stubs
void create_shrapnel(AnimationType type, void* metadata) {}
#endif

static AnimationProperty* d = NULL;


static void animation_def(const char* name, AnimationMetadataType metadata_type)
{
    d = animation_data->get_next();
    IF_ASSERT(d == NULL) return;

    d->set_name(name);

    d->metadata_type = metadata_type;
    switch (metadata_type)
    {
        case AnimDataNone:
            d->metadata = NULL;
            break;

        case AnimDataSimple:
            d->metadata = (class AnimationStateMetadata*)calloc(1, sizeof(class AnimationStateMetadata));
            break;

        default:
            GS_ASSERT(false);
            break;
    }
}

static void register_settings()
{
    animation_def("blue_mining_laser", AnimDataNone);
    d->callback = &dummy;
    animation_def("red_mining_laser", AnimDataNone);
    d->callback = &dummy;
    animation_def("green_mining_laser", AnimDataNone);
    d->callback = &dummy;

    animation_def("plasma_grenade_explode", AnimDataSimple);
    d->callback = &create_shrapnel;
    d->count = 45;
    d->max_count = 60;
    d->use_rand_range = true;
    d->jitter_scale = 1.0f/20.0f;   // applied to starting position
    d->ttl_min = 25;
    d->ttl_max = 35;
    d->momentum = 20.0f;

    animation_def("plasmagen_spur_explode", AnimDataSimple);
    d->callback = &create_shrapnel;
    d->count = 20;
    d->max_count = 30;
    d->use_rand_range = true;
    d->jitter_scale = 1.0f/20.0f;
    d->ttl_min = 20;
    d->ttl_max = 30;
    d->momentum = 20.0f;

    animation_data->done_loading();
}

static void validate_config()
{
    for (size_t i=0; i<animation_data->max; i++)
    {
        AnimationProperty* a = &animation_data->properties[i];
        if (!a->loaded) continue;
        GS_ASSERT(is_valid_name(a->name));
        GS_ASSERT(a->callback != NULL);
        GS_ASSERT(a->metadata_type == AnimDataNone || a->metadata != NULL);
    }

    for (size_t i=0; i<animation_data->max-1; i++)
    for (size_t j=i+1; j<animation_data->max; j++)
    {
        AnimationProperty* a = &animation_data->properties[i];
        AnimationProperty* b = &animation_data->properties[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT(strcmp(a->name, b->name) != 0);
    }
}

void init_config()
{
    GS_ASSERT(animation_data == NULL);
    animation_data = new AnimationProperties;
    register_settings();
    validate_config();
}

void teardown_config()
{
    delete animation_data;
}

class AnimationProperty* get_animation_data(AnimationType type)
{
    return animation_data->get(type);
}

class AnimationProperty* get_animation_data(const char* name)
{
    return animation_data->get(name);
}

anim_callback get_animation_callback(AnimationType animation_type)
{
    class AnimationProperty* data = get_animation_data(animation_type);
    if (data == NULL) return NULL;
    return data->callback;
}

anim_callback get_animation_callback(const char* name)
{
    AnimationType type = get_animation_type(name);
    IF_ASSERT(!isValid(type)) return NULL;
    return get_animation_callback(type);
}

AnimationType get_animation_type(const char* name)
{
    AnimationProperty* p = get_animation_data(name);
    IF_ASSERT(p == NULL) return NULL_ANIMATION_TYPE;
    return p->type;
}

}   // Animations
