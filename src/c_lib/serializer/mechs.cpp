#include "mechs.hpp"

#include <stdint.h>

#include <t_mech/_interface.hpp>

namespace serializer
{

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

        bool placed = t_mech::create_mech(x,y,z, type, subtype);
        GS_ASSERT(placed);
        if (!placed) return false;
    }

    return true;
}

void save_mechs()
{
    // iterate mech list, writing struct

    FILE* f = fopen(mech_filename_tmp, "wb");
    GS_ASSERT(f != NULL);
    if (f == NULL) return;

    bool wrote = write_mech_file(f);
    GS_ASSERT(wrote);
    if (!wrote) return;

    int ret = fclose(f);
    GS_ASSERT(ret == 0);
    if (ret != 0) return;

    if (file_exists(mech_filename))
    {
        ret = rename(mech_filename, mech_filename_backup);
        if (ret != 0) return;
    }
    
    ret = rename(mech_filename_tmp, mech_filename);
    GS_ASSERT(ret == 0);
}

bool load_mechs()
{
    if (file_exists(mech_filename) && fsize(mech_filename) > 0)
        return load_mech_file(mech_filename);
    else
    if (file_exists(mech_filename_backup) && fsize(mech_filename_backup) > 0)
        return load_mech_file(mech_filename_backup);
    else
        printf("WARNING: No mech file found\n");
    return true;
}

}   // serializer
