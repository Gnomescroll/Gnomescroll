#pragma once

#include <serializer/items.hpp>

namespace serializer
{

template<typename F>
F read_bytes(const char* buf, size_t& index)
{
    F val = *((F*)(&buf[index]));
    index += sizeof(F);
    return val;
}

template<typename F>
void write_bytes(char* buf, size_t& index, F val)
{
    *((F*)(&buf[index])) = val;
    index += sizeof(F);
}

// WARNING: modifies char* str
template<class Data>
void parse_line(bool (*process_token) (const char*, const char*, Data*), char* str, size_t length, Data* data)
{
    printf("Parsing line: %s\n", str);
    data->valid = false;
    GS_ASSERT(length);
    if (!length) return;

    // walk the string backwards
    size_t token_length = 0;
    for (int i=(int)length-1; i>=0; i--)
    {
        char c = str[i];
        if (c == ' ') str[i] = '\0';    // convert all spaces to NUL so that padded strings get shortened
        if (i && c != PROPERTY_DELIMITER[0])
        {
            token_length++;
            continue;
        }

        // make sure at least the key + delimiter fits
        GS_ASSERT(token_length >= TAG_LENGTH + TAG_DELIMITER_LENGTH);
        if (token_length < TAG_LENGTH + TAG_DELIMITER_LENGTH) return;

        char* key = NULL;
        if (i)
        {   // we are pointing at a token delimiter, the key starts on the next line
            str[i] = '\0';
            key = &str[i+1];
        }
        else  // we are at position 0, we can't go back any further
            key = &str[i];

        // check that the tag delimiter is correct
        GS_ASSERT(key[TAG_LENGTH] == TAG_DELIMITER[0]);
        if (key[TAG_LENGTH] != TAG_DELIMITER[0]) return;
        
        // replace the tag delimiter with NUL
        key[TAG_LENGTH] = '\0';

        // index to the value
        char* val = &key[TAG_LENGTH + TAG_DELIMITER_LENGTH];
        if (!process_token(key, val, data)) return;

        token_length = 0;
    }
    
    data->valid = true;
}

template<>
void parse_line(bool (*process_token) (const char*, const char*, class ParsedItemData*), char* str, size_t length, class ParsedItemData* data)
{
    printf("Parsing line: %s\n", str);
    data->valid = false;
    GS_ASSERT(length);
    if (!length) return;

    // walk the string backwards
    size_t token_length = 0;
    for (int i=(int)length-1; i>=0; i--)
    {
        char c = str[i];
        if (c == ' ') str[i] = '\0';    // convert all spaces to NUL so that padded strings get shortened
        if (i && c != PROPERTY_DELIMITER[0])
        {
            token_length++;
            continue;
        }

        // make sure at least the key + delimiter fits
        GS_ASSERT(token_length >= TAG_LENGTH + TAG_DELIMITER_LENGTH);
        if (token_length < TAG_LENGTH + TAG_DELIMITER_LENGTH) return;

        char* key = NULL;
        if (i)
        {   // we are pointing at a token delimiter, the key starts on the next line
            str[i] = '\0';
            key = &str[i+1];
        }
        else  // we are at position 0, we can't go back any further
            key = &str[i];

        // check that the tag delimiter is correct
        GS_ASSERT(key[TAG_LENGTH] == TAG_DELIMITER[0]);
        if (key[TAG_LENGTH] != TAG_DELIMITER[0]) return;
        
        // replace the tag delimiter with NUL
        key[TAG_LENGTH] = '\0';

        // index to the value
        char* val = &key[TAG_LENGTH + TAG_DELIMITER_LENGTH];
        if (!process_token(key, val, data)) return;

        printf("Inside parser, key %s, val %s\n", key, val);
        printf("Inside parser Container slot: %d\n", data->container_slot);

        token_length = 0;
    }
    
    data->valid = true;
}

// Parses a newline delimited block of tokens, terminated with a separator
template<class Data>
size_t parse_block(bool (*process_token) (const char*, const char*, Data*), const char* terminator, const char* str, Data* data)
{
    data->reset();
    data->valid = false;
    const size_t LONGEST_LINE = 0x7F;
    static char buf[LONGEST_LINE+1] = {'\0'};
    size_t i = 0;
    while (1)
    {
        char c = '\0';
        size_t n = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && n < LONGEST_LINE)
            buf[n++] = c;
        buf[n] = '\0';
        GS_ASSERT(c == '\n');
        if (c != '\n') return i;
        if (strcmp(buf, terminator) == 0) break;

        GS_ASSERT(n > TAG_LENGTH + TAG_DELIMITER_LENGTH);
        if (n <= TAG_LENGTH + TAG_DELIMITER_LENGTH) return i;
        GS_ASSERT(buf[TAG_LENGTH] == TAG_DELIMITER[0]);
        if (buf[TAG_LENGTH] != TAG_DELIMITER[0]) return i;
        buf[TAG_LENGTH] = '\0';

        const char* key = &buf[0];
        const char* token = &buf[TAG_LENGTH+1];

        if (!process_token(key, token, data)) return i;
    }
    data->valid = true;
    return i;
}

struct MapPosition
{
    int x,y,z;
};

static bool parse_map_position(const char* val, struct MapPosition* position)
{
    bool err = false;
    static char buf[MAP_POSITION_LENGTH+1] = {'\0'};
    strncpy(buf, val, MAP_POSITION_LENGTH);
    buf[MAP_POSITION_LENGTH] = '\0';
    int pts = 1;
    char d;
    int j = 0;
    int cmp_len = 0;
    while ((d = buf[j]) != '\0')
    {
        if (d == MAP_POSITION_COMPONENT_DELIMITER[0])
        {
            GS_ASSERT(cmp_len == MAP_POSITION_COMPONENT_LENGTH);
            if (cmp_len != MAP_POSITION_COMPONENT_LENGTH) return false;
            cmp_len = 0;
            pts++;
            buf[j] = '\0';
        }
        else
            cmp_len++;
        j++;
    }
    GS_ASSERT(cmp_len == MAP_POSITION_COMPONENT_LENGTH);
    if (cmp_len != MAP_POSITION_COMPONENT_LENGTH) return false;
    GS_ASSERT(pts == 3);
    if (pts != 3) return false;
    
    int base_offset = MAP_POSITION_COMPONENT_LENGTH + MAP_POSITION_COMPONENT_DELIMITER_LENGTH;
    long long x = parse_int(&buf[0 * base_offset], err);
    GS_ASSERT(!err && x >= 0 && x < XMAX);
    if (err || x < 0 || x >= XMAX) return false;
    long long y = parse_int(&buf[1 * base_offset], err);
    GS_ASSERT(!err && y >= 0 && y < YMAX);
    if (err || y < 0 || y >= YMAX) return false;
    long long z = parse_int(&buf[2 * base_offset], err);
    GS_ASSERT(!err && z >= 0 && z < ZMAX);
    if (err || z < 0 || z >= ZMAX) return false;

    position->x = x;
    position->y = y;
    position->z = z;
    return (!err);
}

}   // serializer
