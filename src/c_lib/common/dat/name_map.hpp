#pragma once

#include <common/files.hpp>

// No names can be reused ever
// 2 files: active names and dead names

/* Writes dat names to file */

template <class Property>
bool save_active_names(Property* properties, size_t count, size_t name_len, const char* filename)
{
    bool worked = false;

    const char tmp[] = ".tmp";
    char* tmp_filename = (char*)malloc((strlen(filename) + sizeof(tmp)) * sizeof(char));
    sprintf(tmp_filename, "%s%s", filename, tmp);

    const char bak[] = ".bak";
    char* bak_filename = (char*)malloc((strlen(filename) + sizeof(bak)) * sizeof(char));
    sprintf(bak_filename, "%s%s", filename, bak);

    char* buf = (char*)malloc((name_len+2) * sizeof(char));

    FILE* f = fopen(tmp_filename, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL)
        goto error;

    for (size_t i=0; i<count; i++)
    {
        if (!properties[i].loaded) continue;
        size_t len = strlen(properties[i].name);
        GS_ASSERT(len > 0 && len <= name_len);
        if (len <= 0 || len > name_len)
            goto error;
        strncpy(buf, properties[i].name, len);
        buf[len] = '\n';
        buf[len+1] = '\0';
        size_t wrote = fwrite(buf, sizeof(char), len+1, f);
        GS_ASSERT(wrote == len + 1);
        if (wrote != len + 1)
            goto error;
    }

    worked = true;

    error:  // ERROR LABEL

    if (f != NULL)
    {
        worked = (fclose(f) == 0);
        GS_ASSERT(worked);
    }

    if (worked)
    {
        worked = save_tmp_file(filename, tmp_filename, bak_filename);
        GS_ASSERT(worked);
    }

    free(tmp_filename);
    free(bak_filename);
    free(buf);

    return worked;
}

/* Forward-compatible name mapper */

class DatNameMap
{
    public:
        const size_t max;
        const size_t name_max;

        size_t size;
        char* originals;
        char* replacements;

        bool condensed;

    bool add_definition(const char* original, const char* replacement)
    {
        int cmp = strcmp(original, replacement);

        GS_ASSERT(original[0] != '\0');
        GS_ASSERT(replacement[0] != '\0');
        GS_ASSERT(this->size < this->max);
        GS_ASSERT(cmp != 0);

        if (original[0] == '\0') return false;
        if (replacement[0] == '\0') return false;
        if (this->size >= this->max) return false;
        if (cmp == 0) return false;

        // make sure the original name is not duplicated
        for (size_t i=0; i<this->size; i++)
        {
            size_t index = this->get_index(i);
            int cmp = strcmp(original, &this->originals[index]);
            GS_ASSERT(cmp != 0);
            if (cmp == 0) return false;
        }

        size_t index = this->get_index(this->size);
        strncpy(&this->originals[index], original, this->name_max);
        this->originals[index + this->name_max] = '\0';
        strncpy(&this->replacements[index], replacement, this->name_max);
        this->replacements[index + this->name_max] = '\0';
        this->size++;

        return true;
    }

    const char* get_mapped_name(const char* original)
    {
        for (size_t i=0; i<this->size; i++)
        {
            size_t index = this->get_index(i);
            if (strcmp(&this->originals[index], original) == 0)
                return &this->replacements[index];
        }
        return NULL;
    }

    void condense()
    {
        // condense transitive relationships
        // e.g. if A->B and B->C, then change to A->C
        // repeatedly apply until the mapping is stable
        // i.e. no string in replacements is also in originals
        this->condensed = true;
        do
        {
            this->condensed = true;
            for (size_t i=0; i<this->size; i++)
            {
                size_t index = this->get_index(i);
                const char* replacement = &this->replacements[index];
                const char* dest = this->get_mapped_name(replacement);
                if (dest == NULL) continue;

                // make sure name does not resolve to self, for circular definitions
                bool same = (strcmp(&this->originals[index], dest) == 0);
                if (same)
                    printf("ERROR: chained name mapping resolves to self, for name %s\n", dest);
                GS_ASSERT_ABORT(!same);

                strncpy(&this->replacements[index], dest, this->name_max+1);
                this->replacements[index+this->name_max] = '\0';
                this->condensed = false;
            }
        }
        while (!this->condensed);
    }

    bool save(const char* filename)
    {
        bool worked = false;

        const char line_ending[] = "\n";

        const char tmp[] = ".tmp";
        char* tmp_filename = (char*)malloc((strlen(filename) + sizeof(tmp)) * sizeof(char));
        sprintf(tmp_filename, "%s%s", filename, tmp);

        const char bak[] = ".bak";
        char* bak_filename = (char*)malloc((strlen(filename) + sizeof(bak)) * sizeof(char));
        sprintf(bak_filename, "%s%s", filename, bak);

        FILE* f = fopen(tmp_filename, "w");
        GS_ASSERT(f != NULL);
        if (f == NULL)
            goto error;

        for (size_t i=0; i<this->size; i++)
        {
            size_t index = this->get_index(i);
            size_t len = strlen(&this->originals[index]);
            size_t wrote = fwrite(&this->originals[index], sizeof(char), len, f);
            GS_ASSERT(wrote == len);
            if (wrote != len)
                goto error;
            wrote = fwrite(line_ending, sizeof(char), sizeof(line_ending)-1, f);
            GS_ASSERT(wrote == sizeof(line_ending)-1);
            if (wrote != sizeof(line_ending)-1)
                goto error;
        }

        worked = true;

        error:  // ERROR LABEL

        if (f != NULL)
        {
            worked = (fclose(f) == 0);
            GS_ASSERT(worked);
        }

        if (worked)
        {
            worked = save_tmp_file(filename, tmp_filename, bak_filename);
            GS_ASSERT(worked);
        }

        free(tmp_filename);
        free(bak_filename);

        return worked;
    }

    size_t get_index(size_t index)
    {
        return index * (this->name_max+1);
    }

    const char* get_original(size_t i)
    {
        GS_ASSERT(i < this->size);
        if (i >= this->size) return NULL;
        return &this->originals[this->get_index(i)];
    }

    const char* get_replacement(size_t i)
    {
        GS_ASSERT(i < this->size);
        if (i >= this->size) return NULL;
        return &this->replacements[this->get_index(i)];
    }

    DatNameMap(const size_t max, const size_t name_max) :
        max(max), name_max(name_max), size(0), condensed(false)
    {
        this->originals = (char*)calloc(this->max * (this->name_max+1), sizeof(char));
        this->replacements = (char*)calloc(this->max * (this->name_max+1), sizeof(char));
    }

    ~DatNameMap()
    {
        if (this->originals != NULL) free(this->originals);
        if (this->replacements != NULL) free(this->replacements);
    }
};

// caller must free the strings pointed to by active/inactive if not NULL
// creates a buffer, (name_len+1) * _count long, with each name at a fixed point within the buffer
// name_len + 1 bytes apart
char* read_names(const char* filename, const size_t name_length, size_t* name_count)
{
    *name_count = 0;

    if (!file_exists(filename)) return NULL;

    size_t size = 0;
    char* buf = read_file_to_buffer(filename, &size);
    if (buf == NULL) return NULL;

    char* lines = NULL;
    bool read = read_fixed_lines(buf, name_length, &lines, name_count);
    free(buf);

    GS_ASSERT(read);
    if (!read) return NULL;
    return lines;
}

static bool string_in_fixed_lines(const char* str, const char* lines, size_t line_count, size_t line_width)
{
    for (size_t i=0; i<line_count; i++)
    {
        size_t index = (line_width+1) * i;
        const char* line = &lines[index];
        if (strcmp(str, line) == 0) return true;
    }
    return false;
}

template <class Property>
bool name_changes_valid(const char* active_filename, const char* inactive_filename, const size_t name_length, Property* properties, size_t property_count, class DatNameMap* map)
{
    /*
        Rules;
        A new name can be added to active
        An active name can only be moved to inactive
        No names can be removed from inactive

        In other words,

        1. All loaded names must be in either the old active file, or not in the old inactive file
        2. Any name defined in the mapping, must be in either old file.

        3. Any name in the old inactive file must be in the mapping
        4. Any name in the old active file must be in either the mapping or loaded
    */

    bool valid = true;

    size_t active_count = 0;
    size_t inactive_count = 0;

    char* active_names = read_names(active_filename, name_length, &active_count);
    char* inactive_names = read_names(inactive_filename, name_length, &inactive_count);

    // Rule 0: No name should appear in both active and inactive files
    // (This should already be confirmed for the loaded dat data)
    if (active_names != NULL && inactive_names != NULL)
    {
        for (size_t i=0; i<active_count; i++)
        for (size_t j=0; j<inactive_count; j++)
        {
            size_t active_index = (name_length+1) * i;
            size_t inactive_index = (name_length+1) * j;
            if (strcmp(&active_names[active_index], &inactive_names[inactive_index]) == 0)
            {
                printf("ERROR: name %s appears in both %s and %s name files\n", &active_names[active_index], active_filename, inactive_filename);
                valid = false;
            }
        }
    }

    // Rule 1: All loaded names must be in either the old active file, or not in the old inactive file
    for (size_t i=0; i<property_count; i++)
    {
        if (!properties[i].loaded) continue;
        bool in_active = (active_names != NULL && string_in_fixed_lines(properties[i].name, active_names, active_count, name_length));
        bool in_inactive = (inactive_names != NULL && string_in_fixed_lines(properties[i].name, inactive_names, inactive_count, name_length));
        if (!in_active && in_inactive)
        {
            printf("ERROR: name %s does not appear in %s but appears in %s\n", properties[i].name, active_filename, inactive_filename);
            valid = false;
        }
    }

    //  Rule 2: Any name defined in the mapping, must be in either old file.
    for (size_t i=0; i<map->size; i++)
    {
        if ((active_names == NULL || !string_in_fixed_lines(map->get_original(i), active_names, active_count, name_length))
         && (inactive_names == NULL || !string_in_fixed_lines(map->get_original(i), inactive_names, inactive_count, name_length)))
        {
            printf("ERROR: deprecated name %s does not appear in either %s or %s\n", map->get_original(i), active_filename, inactive_filename);
            valid = false;
        }
    }

    // Rule 3: Any name in the old inactive file must be in the mapping
    if (inactive_names != NULL)
    {
        for (size_t i=0; i<inactive_count; i++)
        {
            size_t index = (name_length+1) * i;
            if (map->get_mapped_name(&inactive_names[index]) == NULL)
            {
                printf("ERROR: serialized inactive name %s from file %s not found in the deprecated name definitions\n", &inactive_names[index], inactive_filename);
                valid = false;
            }
        }
    }

    // Rule 4: Any name in the old active file must be in either the mapping or loaded
    if (active_names != NULL)
    {
        for (size_t i=0; i<active_count; i++)
        {
            size_t index = (name_length+1) * i;
            bool found = false;
            for (size_t i=0; i<property_count; i++)
                if (properties[i].loaded && strcmp(properties[i].name, &active_names[index]) == 0)
                {
                    found = true;
                    break;
                }

            if (!found && map->get_mapped_name(&active_names[index]) == NULL)
            {
                printf("ERROR: serialized active name %s from file %s was not found in loaded mapping or loaded definitions\n", &active_names[index], active_filename);
                valid = false;
            }
        }
    }

    if (active_names != NULL) free(active_names);
    if (inactive_names != NULL) free(inactive_names);

    return valid;
}
