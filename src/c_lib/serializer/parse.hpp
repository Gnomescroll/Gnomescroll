#pragma once

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

}   // serializer
