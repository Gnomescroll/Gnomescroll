#include "_interface.hpp"

#include <animations/config/_state.hpp>

#if DC_CLIENT
# include <animations/animations.hpp>
#endif

namespace Animations
{

#if DC_SERVER
// stubs
void create_shrapnel(int id, void* metadata) {}
#endif

static AnimationData* d = NULL;
static int anim_index = 0;

static void add_animation(class AnimationData* d)
{
    IF_ASSERT(d == NULL) return;
    d->loaded = true;
}

static void animation_def(const char* name, AnimationMetadataType metadata_type)
{
    GS_ASSERT(anim_index < MAX_ANIMATIONS);
    IF_ASSERT(anim_index >= MAX_ANIMATIONS) return;
    IF_ASSERT(!is_valid_animation_name(name)) return;
    
    d = &animation_data[anim_index];
    d->id = anim_index;

    strncpy(d->name, name, ANIMATION_NAME_MAX_LENGTH+1);
    d->name[ANIMATION_NAME_MAX_LENGTH] = '\0';

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
    d->metadata_type = metadata_type;
    d->loaded = true;

    anim_index++;
}

static void register_settings()
{
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

    add_animation(d);    // finalize
}

static void validate_config()
{
    for (int i=0; i<MAX_ANIMATIONS; i++)
    {
        if (!animation_data[i].loaded) continue;
        GS_ASSERT(is_valid_animation_name(animation_data[i].name));
        GS_ASSERT(animation_data[i].callback != NULL);
        GS_ASSERT(animation_data[i].metadata_type == AnimDataNone || animation_data[i].metadata != NULL);
    }

    for (int i=0; i<MAX_ANIMATIONS-1; i++)
    for (int j=i+1; j<MAX_ANIMATIONS; j++)
    {
        if (!animation_data[i].loaded) continue;
        if (!animation_data[j].loaded) continue;
        GS_ASSERT(strcmp(animation_data[i].name, animation_data[j].name) != 0);
    }
}

void init_config()
{
    GS_ASSERT(animation_data == NULL);
    animation_data = new AnimationData[MAX_ANIMATIONS];
    
    register_settings();
    validate_config();
}

void teardown_config()
{
    if (animation_data != NULL) delete[] animation_data;
}

class AnimationData* get_animation_data(int animation_id)
{
    IF_ASSERT(animation_data == NULL) return NULL;
    IF_ASSERT(animation_id < 0 || animation_id >= MAX_ANIMATIONS) return NULL;
    IF_ASSERT(!animation_data[animation_id].loaded) return NULL;

    return &animation_data[animation_id];    
}

anim_callback get_animation_callback(int animation_id)
{
    class AnimationData* data = get_animation_data(animation_id);
    if (data == NULL) return NULL;
    return data->callback;
}

anim_callback get_animation_callback(const char* name)
{
    int id = get_animation_id(name);
    if (id < 0) return NULL;
    return get_animation_callback(id);
}

int get_animation_id(const char* name)
{
    for (int i=0; i<MAX_ANIMATIONS; i++)
        if (animation_data[i].loaded && strcmp(animation_data[i].name, name) == 0)
            return i;

    printf("WARNING: No animation found with name %s\n", name);
    return -1;
}

}   // Animations
