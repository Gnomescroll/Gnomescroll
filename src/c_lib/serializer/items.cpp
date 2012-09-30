#include "items.hpp"

#include <stdio.h>
#include <stdint.h>

#include <common/files.hpp>
#include <item/_interface.hpp>

namespace serializer
{

// TODO -- record all errors to a file. email? save the data too?
// TODO -- memcpy all data to a thread
// TODO -- write to .bak file first, then copy over
// TODO -- move invalid parsed files to "invalid" subfolder
// TODO -- apply rename mappings


uint32_t item_global_id = 0;

typedef enum
{
    ITEM_PARSE_TOKEN_ID = 0,
    ITEM_PARSE_TOKEN_TYPE,
    ITEM_PARSE_TOKEN_LOCATION,
    ITEM_PARSE_TOKEN_DURABILITY,
    ITEM_PARSE_TOKEN_STACK_SIZE,
}   ItemParseToken;

const int TOKEN_LENGTH = 3;
const int N_TOKENS = ITEM_PARSE_TOKEN_STACK_SIZE + 1;

const char ITEM_PARSE_TOKENS[N_TOKENS][TOKEN_LENGTH+1] =
{
    #define ITEM_ID_TOKEN         "GID"
             ITEM_ID_TOKEN,
             
    #define ITEM_TYPE_TOKEN       "TYP"
             ITEM_TYPE_TOKEN,
             
    #define ITEM_LOC_TOKEN        "LOC"
             ITEM_LOC_TOKEN,
             
    #define ITEM_DURABILITY_TOKEN "DUR"
             ITEM_DURABILITY_TOKEN,
             
    #define ITEM_STACK_SIZE_TOKEN "STK"
             ITEM_STACK_SIZE_TOKEN
};

const int ITEM_PARSE_TOKEN_PARTS[] =
{ 
    #define ITEM_ID_TOKEN_PARTS         1
             ITEM_ID_TOKEN_PARTS,

    #define ITEM_TYPE_TOKEN_PARTS       1
             ITEM_TYPE_TOKEN_PARTS,

    #define ITEM_LOC_TOKEN_PARTS        3
             ITEM_LOC_TOKEN_PARTS,
             
    #define ITEM_DURABILITY_TOKEN_PARTS 1
             ITEM_DURABILITY_TOKEN_PARTS,
             
    #define ITEM_STACK_SIZE_TOKEN_PARTS 1
             ITEM_STACK_SIZE_TOKEN_PARTS
};

class ParseItemData
{
    public:
    
        uint32_t global_id;
        char name[Item::ITEM_NAME_MAX_LENGTH+1];
        ItemLocationType location;
        int location_id;
        int container_slot;
        int durability;
        int stack_size;

    SerializerError check_values()
    {
        if (this->global_id <= 0)
            return SE_LOAD_ITEM_INVALID_GLOBAL_ID;
        if (this->durability <= 0 || this->durability > NULL_DURABILITY)
            return SE_LOAD_ITEM_INVALID_DURABILITY;
        if (this->stack_size <= 0 || this->stack_size > NULL_STACK_SIZE)
            return SE_LOAD_ITEM_INVALID_STACK_SIZE;
        if (!Item::is_valid_location_data(this->location, this->location_id, this->container_slot, 0))
            return SE_LOAD_ITEM_INVALID_LOCATION;
        if (!Item::is_valid_item_name(this->name))
            return SE_LOAD_ITEM_INVALID_NAME;

        return SE_NONE;
    }

    ParseItemData() :
    global_id(0),
    location(IL_NOWHERE),
    location_id(NULL_LOCATION),
    container_slot(NULL_SLOT),
    durability(NULL_DURABILITY),
    stack_size(NULL_STACK_SIZE)
    {
        memset(name, Item::ITEM_NAME_MAX_LENGTH+1, sizeof(char));
    }
};

static char* item_load_buffer = NULL;
static size_t item_load_buffer_size = 0;

void init_item_serializer()
{
    // starting sizes for the readline buffer
    item_load_buffer_size = 32; 
    item_load_buffer = (char*)malloc(item_load_buffer_size * sizeof(char));

    SerializerError err = load_item_global_id();
    GS_ASSERT(err == SE_NONE);
}

void teardown_item_serializer()
{
    SerializerError err = write_item_global_id();
    GS_ASSERT(err == SE_NONE);

    if (item_load_buffer != NULL) free(item_load_buffer);
}

static const char* get_item_filename(uint32_t global_id)
{   // make filename
    static const int fnlen = sizeof(item_filename_fmt) + 10 - 4;
    static char fnbuf[fnlen+1];
    int ret = snprintf(fnbuf, fnlen+1, item_filename_fmt, global_id);
    GS_ASSERT(ret < fnlen+1);   // did we fit the filename into the buffer
    if (ret >= fnlen + 1) return NULL;
    return fnbuf;
}

SerializerError save_item(ItemID item_id)
{
    // get item
    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return SE_SAVE_ITEM_ITEM_NOT_FOUND;
    GS_ASSERT(item_id == item->id);
    if (item_id != item->id) return SE_SAVE_ITEM_ITEM_ID_MISMATCH;
    const char* name = Item::get_item_name(item->type);
    if (name == NULL) return SE_SAVE_ITEM_ITEM_NAME_NOT_FOUND;

    // TODO -- assert valid values on item before writing
    if (item->global_id <= 0) return SE_SAVE_ITEM_INVALID_GLOBAL_ID;
    if (!item->location_valid) return SE_SAVE_ITEM_INVALID_LOCATION_DATA;
    if (item->durability < 0) return SE_SAVE_ITEM_INVALID_DURABILITY;
    if (item->stack_size < 0) return SE_SAVE_ITEM_INVALID_STACK_SIZE;
    if (!Item::is_valid_item_name(name)) return SE_SAVE_ITEM_INVALID_ITEM_NAME;
    
    // get filename
    const char* fn = get_item_filename(item->global_id);
    GS_ASSERT(fn != NULL);
    if (fn == NULL) return SE_SAVE_ITEM_FILENAME_LONG;

    // create file
    FILE* fp = fopen(fn, "w");
    GS_ASSERT(fp != NULL);
    if (fp == NULL) return SE_SAVE_ITEM_NULL_FILE;

    #define HANDLE_ITEM_SAVE_WRITE_FAIL(ret) \
        if (ret < 0) \
        { \
            save_error = SE_SAVE_ITEM_WRITE_DATA_FAILED; \
            fclose(fp); \
            fopen(fn, "w"); \
            fclose(fp); \
            fp = NULL; \
            goto cleanup; \
        }

    // write data
    int ret = -1;
    SerializerError save_error = SE_NONE;
    ret = fprintf(fp, ITEM_ID_TOKEN " %010d\n", item->global_id);
    HANDLE_ITEM_SAVE_WRITE_FAIL(ret)
    
    ret = fprintf(fp, ITEM_TYPE_TOKEN " %s\n", name);
    HANDLE_ITEM_SAVE_WRITE_FAIL(ret)

    ret = fprintf(fp, ITEM_LOC_TOKEN " %d %d %d\n", item->location, item->location_id, item->container_slot);
    HANDLE_ITEM_SAVE_WRITE_FAIL(ret)

    ret = fprintf(fp, ITEM_DURABILITY_TOKEN " %d\n", item->durability);
    HANDLE_ITEM_SAVE_WRITE_FAIL(ret)

    ret = fprintf(fp, ITEM_STACK_SIZE_TOKEN " %d\n", item->stack_size);
    HANDLE_ITEM_SAVE_WRITE_FAIL(ret)

    #undef HANDLE_ITEM_SAVE_WRITE_FAIL

    // cleanup
    cleanup:
        if (fp != NULL) fclose(fp);

    return save_error;
}

static SerializerError parse_item_line(char* line, size_t size, class ParseItemData* data)
{
    // replace spaces with NUL, checking for valid initial token
    char c;
    int i = 0;
    bool space_found = false;
    bool mark_piece = false;
    int token_len = 0;
    int pieces = 0;
    while ((c = line[i++]) != '\0')
    {
        if (!space_found) token_len++; 
        if (mark_piece)
        {   // increment the piece now (a space was found)
            pieces++;
            mark_piece = false;
        }
        if (isspace(c))
        {
            // dont check isspace(c) for the following, because '\n' will get caught by it,
            // and '\n' is not guaranteed to end the last line 
            if (c == ' ') mark_piece = true;    // mark the piece counter on the next cycle
            
            line[i-1] = '\0';
            if (!space_found)
                space_found = true;
        }
    }

    if (token_len != TOKEN_LENGTH)
        return SE_PARSE_ITEM_INVALID_TOKEN_LENGTH;

    // match token against known tokens
    char* token = line;
    int token_id = 0;
    for (token_id=0; token_id<N_TOKENS; token_id++)
        if (strcmp(ITEM_PARSE_TOKENS[token_id], token) == 0)
            break;

    // token wasnt found
    // we could probably recover, but assume the file is fucked up and report it
    if (token_id >= N_TOKENS)
        return SE_PARSE_ITEM_UNRECOGNIZED_TOKEN;

    // the token piece count does not match
    if (ITEM_PARSE_TOKEN_PARTS[token_id] != pieces)
        return SE_PARSE_ITEM_BAD_PIECES_COUNT;

    int index = TOKEN_LENGTH + 1;
    char* piece = &token[index];    // skip to the first piece. its safe, if the line splitter checks ahead of time
    char* endptr = NULL;
    unsigned long uconvert = 0;
    long convert = 0;

    SerializerError parse_error = SE_NONE;
    
    // now pull data from pieces
    switch ((ItemParseToken) token_id)
    {
        case ITEM_PARSE_TOKEN_ID:
            uconvert = strtoul(piece, &endptr, 10);
            if (uconvert > UINT32_MAX || piece[0] == '\0' || endptr[0] != '\0')
                parse_error = SE_PARSE_ITEM_INVALID_GLOBAL_ID;
            data->global_id = (uint32_t)convert;
            break;
            
        case ITEM_PARSE_TOKEN_TYPE:
            strncpy(data->name, piece, Item::ITEM_NAME_MAX_LENGTH+1);
            break;

        case ITEM_PARSE_TOKEN_LOCATION:
            convert = strtol(piece, &endptr, 10);
            if (convert > INT_MAX || convert > INT_MAX || piece[0] == '\0' || endptr[0] != '\0')
                parse_error = SE_PARSE_ITEM_INVALID_LOCATION;
            data->location = (ItemLocationType)convert;
                
            while (piece[index++] != '\0'); // jump to next token
            piece = &piece[index];
            convert = strtol(piece, &endptr, 10);
            if (convert > INT_MAX || piece[0] == '\0' || endptr[0] != '\0')
                parse_error = SE_PARSE_ITEM_INVALID_LOCATION_ID;
            data->location_id = convert;
                
            while (piece[index++] != '\0'); // jump to next token
            piece = &piece[index];
            convert = strtol(piece, &endptr, 10);
            if (convert > INT_MAX || piece[0] == '\0' || endptr[0] != '\0')
                parse_error = SE_PARSE_ITEM_INVALID_CONTAINER_SLOT;
            data->container_slot = convert;
            break;

        case ITEM_PARSE_TOKEN_DURABILITY:
            convert = strtol(piece, &endptr, 10);
            if (convert > INT_MAX || piece[0] == '\0' || endptr[0] != '\0')
                parse_error = SE_PARSE_ITEM_INVALID_DURABILITY;
            data->durability = convert;
            break;

        case ITEM_PARSE_TOKEN_STACK_SIZE:
            convert = strtol(piece, &endptr, 10);
            if (convert > INT_MAX || piece[0] == '\0' || endptr[0] != '\0')
                parse_error = SE_PARSE_ITEM_INVALID_STACK_SIZE;
            data->stack_size = convert;
            break;

        default:
            // this should never ever occur
            parse_error = SE_PARSE_ITEM_IMPOSSIBLE_TOKEN_MATCH;
            break;
    }

    return parse_error;
}

SerializerError load_item(uint32_t global_id)
{
    // get filemae
    const char* fn = get_item_filename(global_id);
    if (fn == NULL) return SE_LOAD_ITEM_FILENAME_LONG;

    // open file
    FILE* fp = fopen(fn, "r");
    if (fp == NULL) return SE_LOAD_ITEM_FILE_NULL;

    Item::Item* item = NULL;
    int item_type = NULL_ITEM_TYPE;
    SerializerError parse_error = SE_NONE;
    class ParseItemData data;

    // parse file line by line
    while (getline(&item_load_buffer, &item_load_buffer_size, fp) >= 0)
    {
        parse_error = parse_item_line(item_load_buffer, item_load_buffer_size, &data);
        if (parse_error != SE_NONE)
            goto cleanup;
    }

    // TODO -- check data values
    parse_error = data.check_values();
    if (parse_error != SE_NONE)
        goto cleanup;

    // TODO -- apply rename mappings

    // get type for name, checking that its valid
    item_type = Item::get_item_type(data.name);
    if (item_type == NULL_ITEM_TYPE)
    {
        parse_error = SE_LOAD_ITEM_NAME_UNRECOGNIZED;
        goto cleanup;
    }
    
    // create the item
    item = Item::create_item(item_type);
    if (item == NULL)
    {
        parse_error = SE_LOAD_ITEM_CREATE_ITEM_FAILED;
        goto cleanup;
    }

    // set the properties
    item->global_id = data.global_id;
    item->location = data.location;
    item->location_id = data.location_id;
    item->container_slot = data.container_slot;
    item->durability = data.durability;
    item->stack_size = data.stack_size;

    // cleanup
    cleanup:
        if (fp != NULL) fclose(fp);

    return parse_error;
}

void save_items()
{
    using Item::item_list;
    GS_ASSERT(item_list != NULL);
    if (item_list == NULL) return;

    // bulk memcpy for threaded write first
    
    for (int i=0; i<item_list->n_max; i++)
        if (item_list->a[i] != NULL)
            save_item((ItemID)i);
}

SerializerError load_item_global_id()
{
    GS_ASSERT(item_global_id == 0);

    if (!file_exists(item_global_id_filename)) return SE_NONE;
    
    int n = -1;
    char* buf = read_file_to_buffer(item_global_id_filename, &n);
    GS_ASSERT(buf != NULL);
    GS_ASSERT(n > 0);

    // TODO -- this is an error if the global id is not found but there are items floating around in the folder
    // item id rollover is not a problem -- we'll have a separate item server by then that will handle it
    
    if (buf == NULL || n <= 0)
    {
        item_global_id = 0;
        return SE_LOAD_ITEM_GLOBAL_ID_FILE_ERROR;
    }

    item_global_id = (int32_t)atoi(buf);
    free(buf);

    GS_ASSERT(item_global_id > 0);
    if (item_global_id <= 0) return SE_LOAD_ITEM_GLOBAL_ID_INVALID;

    return SE_NONE;
}

SerializerError write_item_global_id()
{
    if (item_global_id == 0) return SE_NONE;

    FILE* fp = fopen(item_global_id_filename, "w");
    GS_ASSERT(fp != NULL);
    if (fp == NULL) return SE_WRITE_ITEM_GLOBAL_ID_FILE_ERROR;

    fprintf(fp, "%d", item_global_id);

    fclose(fp);
    
    return SE_NONE;
}

uint32_t get_new_item_global_id()
{
    GS_ASSERT(item_global_id != UINT32_MAX);
    // TODO -- report error
    if (item_global_id == UINT32_MAX) return 0;
    return ++item_global_id;
}

}   // serializer
