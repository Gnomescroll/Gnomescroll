/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace World
{

extern AttributeGroup attributes;

// Boilerplate
void init_attributes();
void teardown_attributes();
void register_attributes();

// Network
#if DC_SERVER
void send_attributes_to_client(ClientID client_id);
#endif

// API
#define SET_ATTRIBUTE_HEADER(KEY, TYPE) \
    void set_attribute(KEY key, TYPE value);

#define GET_ATTRIBUTE_HEADER(KEY, TYPE, NAME) \
    TYPE get_attribute_##NAME(KEY key);

SET_ATTRIBUTE_HEADER(AttributeType, int);
SET_ATTRIBUTE_HEADER(AttributeType, float);
SET_ATTRIBUTE_HEADER(AttributeType, const char*);
SET_ATTRIBUTE_HEADER(const char*, int);
SET_ATTRIBUTE_HEADER(const char*, float);
SET_ATTRIBUTE_HEADER(const char*, const char*);

GET_ATTRIBUTE_HEADER(AttributeType, int, int);
GET_ATTRIBUTE_HEADER(AttributeType, float, float);
GET_ATTRIBUTE_HEADER(AttributeType, const char*, string);
GET_ATTRIBUTE_HEADER(const char*, int, int);
GET_ATTRIBUTE_HEADER(const char*, float, float);
GET_ATTRIBUTE_HEADER(const char*, const char*, string);

#undef SET_ATTRIBUTE_HEADER
#undef GET_ATTRIBUTE_HEADER

}   // World
