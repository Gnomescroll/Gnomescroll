#include "_interface.hpp"

#include <animations/config/_state.hpp>
#if DC_CLIENT
#include <animations/animations.hpp>
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
    GS_ASSERT(d != NULL);
    if (d == NULL) return;

    d->loaded = true;
}

static void animation_def(const char* name, AnimationMetadataType metadata_type)
{
    GS_ASSERT(animation_data != NULL);
    if (animation_data == NULL) return;
    GS_ASSERT(anim_index < MAX_ANIMATIONS);
    if (anim_index >= MAX_ANIMATIONS) return;
    
    d = &animation_data[anim_index];
    d->id = anim_index;
    
    d->hash = strhash(name);
    d->name = (char*)malloc((strlen(name)+1) * sizeof(char));
    strcpy(d->name, name);

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

    anim_index++;
}

static void register_settings()
{
    animation_def("plasma_grenade_explode", AnimDataSimple);
    d->callback = &create_shrapnel;
    d->count = 25;
    d->max_count = 35;
    d->use_rand_range = true;
    d->jitter_scale = 1.0f/20.0f;   // applied to starting position
    d->ttl_min = 15;
    d->ttl_max = 25;
    d->momentum = 20.0f;

    add_animation(d);    // finalize
}

static void validate_config()
{
    GS_ASSERT(animation_data != NULL);
    if (animation_data == NULL) return;
    
    for (int i=0; i<MAX_ANIMATIONS; i++)
    {
        if (!animation_data[i].loaded) continue;
        unsigned int hash = animation_data[i].hash;
        char* name = animation_data[i].name;
        GS_ASSERT(name != NULL);
        if (name == NULL) continue;
        for (int j=0; j<MAX_ANIMATIONS; j++)
        {
            if (i==j) continue;
            if (!animation_data[j].loaded) continue;
            GS_ASSERT(hash != animation_data[j].hash);
            GS_ASSERT(animation_data[j].name != NULL);
            if (animation_data[j].name == NULL) continue;
            GS_ASSERT(strcmp(name, animation_data[j].name) != 0);
        }

        GS_ASSERT(animation_data[i].callback != NULL);
        GS_ASSERT(animation_data[i].metadata_type == AnimDataNone || animation_data[i].metadata != NULL);
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
    GS_ASSERT(animation_data != NULL);
    if (animation_data == NULL) return NULL;
    
    GS_ASSERT(animation_id >= 0 && animation_id < MAX_ANIMATIONS);
    if (animation_id < 0 || animation_id >= MAX_ANIMATIONS) return NULL;

    GS_ASSERT(animation_data[animation_id].loaded);
    if (!animation_data[animation_id].loaded) return NULL;

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
    printf("Getting animation id for %s\n", name);
    GS_ASSERT(animation_data != NULL);
    if (animation_data == NULL) return -1;
    
    unsigned int hash = strhash(name);
    for (int i=0; i<MAX_ANIMATIONS; i++) if (animation_data[i].name != NULL) printf("name: %s\n", animation_data[i].name);
    for (int i=0; i<MAX_ANIMATIONS; i++)
        if (animation_data[i].loaded && animation_data[i].hash == hash)
            return i;
    return -1;
}

}   // Animations
