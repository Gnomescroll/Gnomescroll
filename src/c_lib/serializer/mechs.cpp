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
    if (mech_type_map != NULL) free(mech_type_map);
}

static bool parse_mech_palette_token(const char* key, const char* val, class ParsedMechPaletteData* data)
{
    bool err = false;
    if (strcmp(MECH_TYPE_TAG, key) == 0)
    {
        long long mech_type = parse_int(val, err);
        ASSERT_VALID_MECH_TYPE(mech_type);
        IF_INVALID_MECH_TYPE(mech_type) return false;
        GS_ASSERT(!err);
        if (err) return false;
        data->mech_type = (MechType)mech_type;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = t_mech::is_valid_mech_name(val);
        GS_ASSERT(valid_name);
        if (!valid_name) return false;
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
    GS_ASSERT(str != NULL)
    GS_ASSERT(size > 0);
    if (str == NULL) return false;
    if (size <= 0)
    {
        free(str);
        return false;
    }

    // allocate scratch buffer long enough to hold the largest line
    static const size_t LONGEST_LINE = MECH_PALETTE_LINE_LENGTH;
    char buf[LONGEST_LINE+1] = {'\0'};

    size_t istr = 0;
    class ParsedMechPaletteData palette_data;
    while (istr < size)
    {    
        // copy line
        size_t ibuf = 0;
        char c = '\0';
        while ((c = str[istr++]) != '\0' && c != '\n' && ibuf < LONGEST_LINE)
            buf[ibuf++] = c;
        buf[ibuf] = '\0';
        GS_ASSERT(c == '\n' || c == '\0');
        if (c != '\0' && c != '\n')
        {
            free(str);
            return false;
        }

        parse_line<class ParsedMechPaletteData>(&parse_mech_palette_token, buf, ibuf, &palette_data);
        GS_ASSERT(palette_data.valid);
        if (!palette_data.valid)
        {
            free(str);
            return false;
        }

        MechType actual_mech_type = t_mech::get_compatible_mech_type(palette_data.name);
        GS_ASSERT(actual_mech_type != NULL_MECH_TYPE);
        if (actual_mech_type == NULL_MECH_TYPE)
        {   // we failed to get a compatible mech type
            free(str);
            return false;
        }
        GS_ASSERT(mech_type_map[palette_data.mech_type] = NULL_MECH_TYPE);
        if (mech_type_map[palette_data.mech_type] != NULL_MECH_TYPE)
        {   // we already loaded this mech (duplicate entry)
            free(str);
            return false;
        }
        
        mech_type_map[palette_data.mech_type] = actual_mech_type;
        
        if (c == '\0') break;
    }

    free(str);

    return true;
}

bool save_mech_palette_file()
{
    FILE* f = fopen(mech_palette_filename_tmp, "w");
    GS_ASSERT(f != NULL);
    if (f == NULL) return false;

    char buf[MECH_PALETTE_LINE_LENGTH+2] = {'\0'};

    for (int i=0; i<MAX_MECHS; i++)
    {
        if (!t_mech::get_mech_type_in_use((MechType)i)) continue;
        const char* mech_name = t_mech::get_mech_name((MechType)i);
        if (mech_name == NULL) continue;
        int len = snprintf(buf, MECH_PALETTE_LINE_LENGTH+1, MECH_PALETTE_FMT, mech_name, i);
        GS_ASSERT(len >= 0 && (size_t)len < MECH_PALETTE_LINE_LENGTH+1);
        if (len < 0 || (size_t)len >= MECH_PALETTE_LINE_LENGTH+1) return false;
        buf[len++] = '\n';
        buf[len] = '\0';
        
        size_t wrote = fwrite(buf, sizeof(char), (size_t)len, f);
        GS_ASSERT(wrote == (size_t)len);
        if (wrote != (size_t)len) return false;
    }

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    if (ret != 0) return false;

    return save_file(mech_palette_filename, mech_palette_filename_tmp, mech_palette_filename_backup);
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
    GS_ASSERT(wrote == 1);
    if (wrote != 1) return false;

    // write mech data
    size_t actual_mech_count = 0;
    for (int i=0; i<mech_list->mlm; i++)
    {
        index = 0;
        struct MECH* mech = &mech_list->mla[i];
        if (mech->id == -1) continue;
        ASSERT_VALID_MECH_TYPE(mech->mech_type);
        GS_ASSERT(mech->subtype >= 0 && (unsigned int)mech->subtype <= UINT8_MAX);
        GS_ASSERT(mech->x >= 0 && mech->x < XMAX);
        GS_ASSERT(mech->y >= 0 && mech->y < YMAX);
        GS_ASSERT(mech->z >= 0 && mech->z < ZMAX);

        IF_INVALID_MECH_TYPE(mech->mech_type) continue;
        if (mech->subtype < 0 || (unsigned int)mech->subtype > UINT8_MAX) continue;
        if (mech->x < 0 || mech->x >= XMAX) continue;
        if (mech->y < 0 || mech->y >= YMAX) continue;
        if (mech->z < 0 || mech->z >= ZMAX) continue;

        write_bytes<uint8_t>(buf, index, mech->mech_type);
        write_bytes<uint8_t>(buf, index, mech->subtype);
        write_bytes<uint16_t>(buf, index, mech->x);
        write_bytes<uint16_t>(buf, index, mech->y);
        write_bytes<uint16_t>(buf, index, mech->z);

        #if !PRODUCTION
        GS_ASSERT(index <= buflen);
        if (index > buflen) return false;   // this is a programming error
        #endif

        wrote = fwrite(buf, sizeof(char), index, f);
        GS_ASSERT(wrote == buflen);
        if (wrote != buflen) return false;

        actual_mech_count++;
    }

    GS_ASSERT(mech_count == actual_mech_count);

    // write actual count, if different
    if (mech_count != actual_mech_count)
    {
        rewind(f);
        index = 0;
        write_bytes<uint32_t>(buf, index, actual_mech_count);
        wrote = fwrite(buf, sizeof(uint32_t), 1, f);
        GS_ASSERT(wrote == 1);
        if (wrote != 1) return false;
    }
    
    return true;
}

bool load_mech_file(const char* fn)
{
    printf("Loading mech file %s\n", fn);
    
    size_t size = 0;
    char* buf = read_binary_file_to_buffer(fn, &size);
    GS_ASSERT(buf != NULL);
    if (buf == NULL) return false;

    // file should be larger than at least the mech count
    GS_ASSERT(size >= sizeof(uint32_t));
    if (size < sizeof(uint32_t)) return false;

    size_t ibuf = 0;
    uint32_t mech_count = ((uint32_t*)buf)[ibuf];
    ibuf += sizeof(uint32_t);

    GS_ASSERT(mech_count >= 0);
    if (mech_count < 0) return false;

    static const size_t mech_size = 3*sizeof(uint8_t) + 2*sizeof(uint16_t);
    for (size_t i=0; i<mech_count; i++)
    {
        GS_ASSERT(size - ibuf >= mech_size);
        if (size - ibuf < mech_size) return false;

        int type = read_bytes<uint8_t>(buf, ibuf);
        int subtype = read_bytes<uint8_t>(buf, ibuf);
        int x = read_bytes<uint16_t>(buf, ibuf);
        int y = read_bytes<uint16_t>(buf, ibuf);
        int z = read_bytes<uint16_t>(buf, ibuf);
        
        ASSERT_VALID_MECH_TYPE(type);
        GS_ASSERT(subtype >= 0);
        GS_ASSERT(x >= 0 && x < XMAX);
        GS_ASSERT(y >= 0 && y < YMAX);
        GS_ASSERT(z >= 0 && z < ZMAX);

        if (x < 0 || x >= XMAX) return false;
        if (y < 0 || y >= YMAX) return false;
        if (z < 0 || z >= ZMAX) return false;
        IF_INVALID_MECH_TYPE(type) return false;
        if (subtype < 0) return false;

        // apply renaming
        MechType mech_type = mech_type_map[type];
        ASSERT_VALID_MECH_TYPE(mech_type);
        IF_INVALID_MECH_TYPE(mech_type) return false;

        bool placed = t_mech::create_mech(x,y,z, mech_type, subtype);
        GS_ASSERT(placed);
        if (!placed) return false;
    }

    return true;
}

bool save_mechs()
{
    bool saved_palette = save_mech_palette_file();
    GS_ASSERT(saved_palette);
    if (!saved_palette) return false;
    
    FILE* f = fopen(mech_filename_tmp, "wb");
    GS_ASSERT(f != NULL);
    if (f == NULL) return false;

    bool wrote = write_mech_file(f);
    GS_ASSERT(wrote);
    if (!wrote) return false;

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    if (ret != 0) return false;

    if (file_exists(mech_filename))
    {
        ret = rename(mech_filename, mech_filename_backup);
        if (ret != 0) return false;
    }
    
    ret = rename(mech_filename_tmp, mech_filename);
    GS_ASSERT(ret == 0);

    return (ret == 0);
}

bool load_mechs()
{    
    if (file_exists(mech_filename) && fsize(mech_filename) > 0)
    {
        if (!load_mech_palette_file(mech_palette_filename)) return false;
        return load_mech_file(mech_filename);
    }
    else
    if (file_exists(mech_filename_backup) && fsize(mech_filename_backup) > 0)
    {
        if (!load_mech_palette_file(mech_palette_filename_backup)) return false;
        return load_mech_file(mech_filename_backup);
    }
    else
        printf("WARNING: No mech file found\n");
    return true;
}

}   // serializer
