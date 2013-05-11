#pragma once

namespace data
{

typedef struct dict_entry_s 
{
    const char *key;
    int value;
} dict_entry_s;

typedef struct dict_s 
{
    int len;
    int cap;
    dict_entry_s *entry;
} dict_s, *dict_t;

int dict_find_index(dict_t dict, const char *key) 
{
    for (int i = 0; i < dict->len; i++) 
	{
        if (!strcmp(dict->entry[i], key)) 
		{
            return i;
        }
    }
    return -1;
}

int dict_find(dict_t dict, const char *key, int def) 
{
    int idx = dict_find_index(dict, key);
    return idx == -1 ? def : dict->entry[idx].value;
}

void dict_add(dict_t dict, const char *key, int value)
 {
   int idx = dict_find_index(dict, key);
   if (idx != -1) 
	{
       dict->entry[idx].value = value;
       return;
   }
   if (dict->len == dict->cap) 
	{
       dict->cap *= 2;
       dict->entry = realloc(dict->entry, dict->cap * sizeof(dict_entry_s));
   }
   dict->entry[dict->len].key = strdup(key);
   dict->entry[dict->len].value = value;
   dict->len++;
}

dict_t dict_new(void) 
{
    dict_s proto = {0, 10, malloc(10 * sizeof(dict_entry_s))};
    dict_t d = malloc(sizeof(dict_s));
    *d = proto;
    return d;
}

void dict_free(dict_t dict) 
{
    for (int i = 0; i < dict->len; i++) 
	{
        free(dict->entry[i].key);
    }
    free(dict->entry);
    free(dict);
}

}

