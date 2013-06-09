#pragma once

#include <t_mob/sprites/common.hpp>
#include <SDL/texture_sheet_loader.hpp>

/* Example use:

SpriteSheet sheet = animations->load_texture("rdn");
SpriteAnimationGroup* g = animations->create(sheet);
IF_ASSERT(g == NULL) return;
SpriteAnimation* a = g->create("walk");
IF_ASSERT(a == NULL) return;
a->add_frame(0, 0);
a->add_frame(1, 0);
a->add_frame_strip(0, 2);

*/

namespace t_mob
{

void init_animation_repo();
void teardown_animation_repo();

class SpriteAnimation
{
    private:

    void _add_frame_strip(size_t x, size_t y, size_t n, bool horizontal);

    public:
        static const size_t MAX_FRAMES = 8;
        static const size_t ANIMATION_NAME_MAX = 31;

        class SpriteAnimationGroup* group;
        SpriteAnimationID id;
        char name[ANIMATION_NAME_MAX+1];

        SpriteSheet sheet;
        size_t count;
        int frames[MAX_FRAMES];

    void add_frame(size_t x, size_t y);
    void add_vertical_frame_strip(size_t x, size_t y, size_t n);
    void add_horizontal_frame_strip(size_t x, size_t y, size_t n);

    void add_frame_strip(size_t x, size_t y, size_t n)
    {
        this->add_horizontal_frame_strip(x, y, n);
    }

    void set_name(const char* name)
    {
        strncpy(this->name, name, ANIMATION_NAME_MAX + 1);
        GS_ASSERT(this->name[ANIMATION_NAME_MAX] == '\0');
        this->name[ANIMATION_NAME_MAX] = '\0';
    }

    SpriteAnimation() :
        group(NULL), id(NULL_SPRITE_ANIMATION), sheet(NULL_SPRITE_SHEET),
        count(0)
    {
        memset(this->name, 0, sizeof(this->name));
        memset(this->frames, 0, sizeof(this->frames));
    }
};

class SpriteAnimationGroup
{
    public:
        static const size_t ANIMATIONS_MAX = 8;

        class SpriteAnimationRepo* repo;
        SpriteAnimationGroupID id;

        SpriteSheet sheet;
        SpriteAnimation animations[ANIMATIONS_MAX];
        size_t count;

    SpriteAnimation* create(const char* name)
    {
        IF_ASSERT(this->count >= ANIMATIONS_MAX) return NULL;
        SpriteAnimation* a = &this->animations[this->count];
        a->group = this;
        a->id = SpriteAnimationID(this->count);
        a->sheet = this->sheet;
        a->set_name(name);
        this->count++;
        return a;
    }

    void verify()
    {
        if (!this->count) return;
        for (size_t i=0; i<this->count-1; i++)
        for (size_t j=i+1; i<this->count; i++)
        {
            GS_ASSERT(strcmp(this->animations[i].name,
                             this->animations[j].name) == 0);
        }
    }

    SpriteAnimationGroup() :
        repo(NULL), id(NULL_SPRITE_ANIMATION_GROUP), count(0)
    {
        GS_ASSERT(ANIMATIONS_MAX < size_t(NULL_SPRITE_ANIMATION));
    }
};


class SpriteAnimationRepo
{
    private:

    static int _cmp_group(const void* a, const void* b)
    {
        const SpriteAnimationGroup* ga = reinterpret_cast<const SpriteAnimationGroup*>(a);
        const SpriteAnimationGroup* gb = reinterpret_cast<const SpriteAnimationGroup*>(b);
        if (ga->sheet == gb->sheet) return 0;
        if (ga->sheet == NULL_SPRITE_SHEET) return 1;
        if (gb->sheet == NULL_SPRITE_SHEET) return -1;
        if (int(ga->sheet) < int(gb->sheet))
            return -1;
        else
            return 1;
    }

    void sort_by_sheet_id()
    {
        qsort(this->groups, this->count, sizeof(*this->groups),
              &SpriteAnimationRepo::_cmp_group);
    }

    public:
        static const size_t ANIMATION_GROUPS_MAX = NULL_SPRITE_ANIMATION_GROUP - 1;

        TextureSheetLoader::TextureSheetLoader texture_loader;
        SpriteAnimationGroup groups[ANIMATION_GROUPS_MAX];
        size_t count;

    SpriteSheet load_texture(const char* name)
    {
        const char fmt[] = MEDIA_PATH "sprites/mob/%s.png";
        const size_t len = sizeof(fmt) + strlen(name);
        char* filename = (char*)malloc(len);
        size_t could = snprintf(filename, len, fmt, name);
        GS_ASSERT(could < len);
        filename[len-1] = '\0';
        SpriteSheet sheet = this->texture_loader.load_texture(filename);
        free(filename);
        return sheet;
    }

    SpriteAnimationGroup* create(SpriteSheet sheet)
    {
        IF_ASSERT(this->count >= ANIMATION_GROUPS_MAX) return NULL;
        SpriteAnimationGroup* g = &this->groups[this->count];
        g->repo = this;
        g->id = SpriteAnimationGroupID(this->count);
        g->sheet = sheet;
        this->count++;
        return g;
    }

    SpriteAnimationGroup* create(const char* sheet_name)
    {
        return this->create(this->load_texture(sheet_name));
    }

    SpriteAnimationGroup* get(SpriteAnimationGroupID id)
    {
        IF_ASSERT(id < 0 || id > ANIMATION_GROUPS_MAX) return NULL;
        IF_ASSERT(this->groups[id].id != id) return NULL;
        return &this->groups[id];
    }

    void verify()
    {
        for (size_t i=0; i<this->count; i++)
            this->groups[i].verify();
    }

    void finish()
    {
        this->sort_by_sheet_id();
        this->verify();
    }

    SpriteAnimationRepo() :
        texture_loader(16), count(0)
    {
        GS_ASSERT(ANIMATION_GROUPS_MAX < size_t(NULL_SPRITE_ANIMATION_GROUP));
    }
};

extern class SpriteAnimationRepo* animations;

}   // t_mob
