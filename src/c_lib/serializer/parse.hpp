#pragma once

namespace serializer
{

// WARNING: modifies char* str
template<class Data>
void parse_line(bool (*process_token) (const char*, const char*, Data*), char* str, size_t length, Data* data)
{
    data->valid = false;
    GS_ASSERT(length);
    if (!length) return;

    int i = (int)(length - 1);
    size_t token_length = 0;
    while (i >= 0)
    {
        char c = str[i];
        if (c == ' ') str[i] = '\0';    // convert all spaces to NUL so that padded strings get shortened
        if (i && c != PROPERTY_DELIMITER[0])
        {
            i--;
            token_length++;
            continue;
        }
        GS_ASSERT(token_length >= TAG_LENGTH + TAG_DELIMITER_LENGTH);
        if (token_length < TAG_LENGTH + TAG_DELIMITER_LENGTH) return;

        char* key = NULL;
        if (i)
        {
            str[i] = '\0';
            key = &str[i];
        }

        char* val = &key[TAG_LENGTH + TAG_DELIMITER_LENGTH];
        if (!process_token(key, val, data)) return;

        i--;
        token_length = 0;
    }
    
    data->valid = true;
}

}   // serializer
