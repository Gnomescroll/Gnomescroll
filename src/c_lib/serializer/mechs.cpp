/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "mechs.hpp"

#include <stdint.h>

#include <t_mech/_interface.hpp>
#include <serializer/_interface.hpp>

namespace serializer
{

static MechType* mech_type_map = NULL;

void init_mechs()
{
    GS_ASSERT(mech_type_map == NULL);
    mech_type_map = (MechType*)malloc(MAX_MECHS * sizeof(MechType));
    for (int i=0; i<MAX_MECHS; mech_type_map[i++] = NULL_MECH_TYPE);
}

void teardown_mechs()
{
    free(mech_type_map);
}

static bool parse_mech_palette_token(const char* key, const char* val, class ParsedMechPaletteData* data)
{
    bool err = false;
    if (strcmp(MECH_TYPE_TAG, key) == 0)
    {
        long long mech_type = parse_int(val, err);
        IF_ASSERT(!isValid((MechType)mech_type)) return false;
        IF_ASSERT(err) return false;
        data->mech_type = (MechType)mech_type;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = t_mech::is_valid_mech_name(val);
        IF_ASSERT(!valid_name) return false;
        strncpy(data->name, val, DAT_NAME_MAX_LENGTH);
        data->name[DAT_NAME_MAX_LENGTH] = '\0';
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }

    return true;
}

bool load_mech_palette_file(const char* fn)
{
    printf("Loading mech palette file %s\n", fn);

    size_t size = 0;
    char* str = read_file_to_buffer(fn, &size);
    IF_ASSERT(str == NULL) return false;

    // allocate scratch buffer long enough to hold the largest line
    static const size_t LONGEST_LINE = MECH_PALETTE_LINE_LENGTH;
    char buf[LONGEST_LINE+1] = {'\0'};

    size_t istr = 0;
    class ParsedMechPaletteData palette_data;
    while (istr < size)
    {   // copy line
        size_t ibuf = 0;
        char c = '\0';
        while ((c = str[istr++]) != '\0' && c != '\n' && ibuf < LONGEST_LINE)
            buf[ibuf++] = c;
        buf[ibuf] = '\0';
        IF_ASSERT(c != '\0' && c != '\n')
        {
            free(str);
            return false;
        }

        parse_line<class ParsedMechPaletteData>(&parse_mech_palette_token, buf, ibuf, &palette_data);
        IF_ASSERT(!palette_data.valid)
        {
            free(str);
            return false;
        }

        const char* actual_name = t_mech::get_compatible_mech_name(palette_data.name);
        IF_ASSERT(actual_name == NULL)
        {
            free(str);
            return false;
        }

        MechType mech_type = t_mech::get_mech_type(actual_name);
        IF_ASSERT(mech_type == NULL_MECH_TYPE)
        {   // we failed to get a compatible mech type
            free(str);
            return false;
        }

        IF_ASSERT(mech_type_map[palette_data.mech_type] != NULL_MECH_TYPE)
        {   // we already loaded this mech (duplicate entry)
            free(str);
            return false;
        }

        mech_type_map[palette_data.mech_type] = mech_type;

        if (c == '\0') break;
    }

    free(str);

    return true;
}

bool save_mech_palette_file()
{
    FILE* f = fopen(mech_palette_path_tmp, "w");
    IF_ASSERT(f == NULL) return false;

    char buf[MECH_PALETTE_LINE_LENGTH+2] = {'\0'};

    for (int i=0; i<MAX_MECHS; i++)
    {
        if (!t_mech::type_in_use((MechType)i)) continue;
        const char* mech_name = t_mech::get_mech_name((MechType)i);
        if (mech_name == NULL) continue;
        int len = snprintf(buf, MECH_PALETTE_LINE_LENGTH+1, MECH_PALETTE_FMT, mech_name, i);
        IF_ASSERT(len < 0 || (size_t)len >= MECH_PALETTE_LINE_LENGTH+1) return false;
        buf[len++] = '\n';
        buf[len] = '\0';

        size_t wrote = fwrite(buf, sizeof(char), (size_t)len, f);
        IF_ASSERT(wrote != (size_t)len) return false;
    }

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    return (ret == 0);
}

bool write_mech_file(FILE* f)
{
    using t_mech::mech_list;

    // TODO -- check mech config stuff, valid mech state etc

    static const size_t buflen = 3*sizeof(uint16_t) + 2*sizeof(uint8_t);
    char buf[buflen];

    // weite number of mechs in file first
    uint32_t mech_count = mech_list->mln;
    size_t index = 0;
    write_bytes<uint32_t>(buf, index, mech_count);
    size_t wrote = fwrite(buf, sizeof(uint32_t), 1, f);
    IF_ASSERT(wrote != 1) return false;

    // write mech data
    size_t actual_mech_count = 0;
    for (int i=0; i<mech_list->mlm; i++)
    {
        index = 0;
        struct Mech* mech = &mech_list->mla[i];
        if (mech->id == -1) continue;

        IF_ASSERT(!isValid(mech->type)) continue;
        IF_ASSERT(mech->subtype < 0 || (unsigned int)mech->subtype > UINT8_MAX) continue;
        IF_ASSERT(!is_boxed_position(mech->position)) continue;

        write_bytes<uint8_t>(buf, index, mech->type);
        write_bytes<uint8_t>(buf, index, mech->subtype);
        write_bytes<uint16_t>(buf, index, mech->position.x);
        write_bytes<uint16_t>(buf, index, mech->position.y);
        write_bytes<uint16_t>(buf, index, mech->position.z);

        #if !PRODUCTION
        IF_ASSERT(index > buflen) return false;   // this is a programming error
        #endif

        wrote = fwrite(buf, sizeof(char), index, f);
        IF_ASSERT(wrote != buflen) return false;

        actual_mech_count++;
    }

    // write actual count, if different
    IF_ASSERT(mech_count != actual_mech_count)
    {
        rewind(f);
        index = 0;
        write_bytes<uint32_t>(buf, index, actual_mech_count);
        wrote = fwrite(buf, sizeof(uint32_t), 1, f);
        IF_ASSERT(wrote != 1) return false;
    }

    return true;
}

bool load_mech_file(const char* fn)
{
    printf("Loading mech file %s\n", fn);
    bool success = false;
    static const size_t mech_size = 3*sizeof(uint8_t) + 2*sizeof(uint16_t);

    size_t size = 0;
    char* buf = read_binary_file_to_buffer(fn, &size);
    IF_ASSERT(buf == NULL) return false;

    size_t ibuf = 0;
    uint32_t mech_count = reinterpret_cast<uint32_t*>(buf)[ibuf];
    ibuf += sizeof(uint32_t);

    // file should be larger than at least the mech count
    IF_ASSERT(size < sizeof(uint32_t)) goto error;

    for (size_t i=0; i<mech_count; i++)
    {
        IF_ASSERT(size - ibuf < mech_size) goto error;

        int type = read_bytes<uint8_t>(buf, ibuf);
        int subtype = read_bytes<uint8_t>(buf, ibuf);
        int x = read_bytes<uint16_t>(buf, ibuf);
        int y = read_bytes<uint16_t>(buf, ibuf);
        int z = read_bytes<uint16_t>(buf, ibuf);

        #define LOG_GOTO_ERROR { \
            log_mech_load_error("Mech values invalid", p, (MechType)type, subtype); \
            goto error;}

        Vec3i p = vec3i_init(x, y, z);
        IF_ASSERT(!is_boxed_position(p)) LOG_GOTO_ERROR;
        IF_ASSERT(!isValid((MechType)type)) LOG_GOTO_ERROR
        IF_ASSERT(subtype < 0) LOG_GOTO_ERROR

        #undef LOG_GOTO_ERROR

        // apply renaming
        MechType mech_type = mech_type_map[type];
        IF_ASSERT(!isValid(mech_type))
        {
            log_mech_load_error("Mech type not valid", p, mech_type, subtype);
            goto error;
        }

        MechCreateFailureCode ret = t_mech::create_mech(p, mech_type, subtype);
        success = (ret == MCF_OK);
        IF_ASSERT(!success)
        {
            t_mech::print_mech_create_failure_code(ret);
            log_mech_load_error("Failed to create mech", p, mech_type, subtype);
        }
    }

    success = true;

    error:  // GOTO LABEL
        free(buf);

    return success;
}

bool save_mechs()
{
    bool saved_palette = save_mech_palette_file();
    IF_ASSERT(!saved_palette) return false;

    FILE* f = fopen(mech_path_tmp, "wb");
    IF_ASSERT(f == NULL) return false;

    bool wrote = write_mech_file(f);
    IF_ASSERT(!wrote) return false;

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    return (ret == 0);
}

bool load_mechs()
{
    if (file_exists(mech_path) && get_filesize(mech_path) > 0)
    {
        if (!load_mech_palette_file(mech_palette_path)) return false;
        return load_mech_file(mech_path);
    }
    else
    if (file_exists(mech_path_bak) && get_filesize(mech_path_bak) > 0)
    {
        if (!load_mech_palette_file(mech_palette_path_bak)) return false;
        return load_mech_file(mech_path_bak);
    }
    else
        printf("WARNING: No mech file found\n");
    return true;
}

}   // serializer
