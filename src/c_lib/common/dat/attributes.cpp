#include "attributes.hpp"

#include <common/dat/properties.hpp>

namespace Attributes
{

class Attribute: public Property<AttributeType>
{
    public:
        AttributeGroup group;
        AttributeValueType value_type;
        AttributeSyncType sync_type;

        void* location;
        voidFunction getter;
        voidFunction setter;

        bool changed;
        ClientID sync_to;

    /* Read/write API */

    int get_int() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return 1;
        if (this->location != NULL)
            return *(reinterpret_cast<int*>(this->location));
        return reinterpret_cast<getInt>(this->getter)();
    }

    float get_float() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return 1.0f;
        if (this->location != NULL)
            return *(reinterpret_cast<float*>(this->location));
        return reinterpret_cast<getFloat>(this->getter)();
    }

    const char* get_string() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return NULL;
        if (this->location != NULL)
            return *(reinterpret_cast<char**>(this->location));
        return reinterpret_cast<getString>(this->getter)();
    }

    void set(int value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return;
        if (this->location != NULL)
        {
            int current = *(reinterpret_cast<int*>(this->location));
            if (current != value)
            {
                *(reinterpret_cast<int*>(this->location)) = value;
                this->changed = true;
            }
        }
        else if (reinterpret_cast<setInt>(this->setter)(value))
            this->changed = true;
    }

    void set(float value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return;
        if (this->location != NULL)
        {
            float current = *(reinterpret_cast<float*>(this->location));
            if (current != value)
            {
                *(reinterpret_cast<float*>(this->location)) = value;
                this->changed = true;
            }
        }
        else
        if (reinterpret_cast<setFloat>(this->setter)(value))
            this->changed = true;
    }

    void set(const char* value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return;
        if (this->location != NULL)
        {
            char* current = *(reinterpret_cast<char**>(this->location));
            if (strcmp(current, value) != 0)
            {
                strncpy(current, value, STRING_ATTRIBUTE_MAX_LENGTH);
                current[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
                this->changed = true;
            }
        }
        else
        if (reinterpret_cast<setString>(this->setter)(value))
            this->changed = true;
    }

    void set_sync_to(ClientID client_id)
    {
        GS_ASSERT(this->sync_to == NULL_CLIENT);
        this->sync_to = client_id;
    }

    /* Registration API */

    void add_setter(setInt setter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_setter(reinterpret_cast<voidFunction>(setter));
    }

    void add_setter(setFloat setter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_setter(reinterpret_cast<voidFunction>(setter));
    }

    void add_setter(setString setter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_setter(reinterpret_cast<voidFunction>(setter));
    }

    void add_getter(setInt getter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_getter(reinterpret_cast<voidFunction>(getter));
    }

    void add_getter(setFloat getter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_getter(reinterpret_cast<voidFunction>(getter));
    }

    void add_getter(setString getter)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_getter(reinterpret_cast<voidFunction>(getter));
    }

    void set_location(int* location)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_set_location(location);
    }

    void set_location(float* location)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_set_location(location);
    }

    void set_location(char** location)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_set_location(location);
    }

    void set_sync_type(AttributeSyncType sync_type)
    {
        GS_ASSERT(this->sync_type == NULL_ATTRIBUTE_SYNC_TYPE);
        this->sync_type = sync_type;
    }

    /* Networking */

    void send() const
    {
        if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PRIVATE) return;

        #define ROUTE_MESSAGE(MSG_TYPE) \
        { do { \
            MSG_TYPE msg; \
            this->_pack_message(&msg); \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_ALL) \
                msg.broadcast(); \
            else \
            if (this->sync_type == ATTRIBUTE_SYNC_TYPE_PLAYER) \
            { \
                GS_ASSERT(this->sync_to != NULL_CLIENT); \
                msg.sendToClient(this->sync_to); \
            } \
        } while (0); }

        if (this->value_type == ATTRIBUTE_VALUE_INT)
        {
            ROUTE_MESSAGE(set_attribute_int_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
        {
            ROUTE_MESSAGE(set_attribute_float_StoC);
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_STRING)
        {
            ROUTE_MESSAGE(set_attribute_string_StoC);
        }

        #undef ROUTE_MESSAGE

        GS_ASSERT(false);
    }

    void send_changes()
    {
        if (!this->changed) return;
        this->send();
        this->changed = false;
    }

    /* Verification */

    void verify() const
    {
        GS_ASSERT(this->type != NULL_ATTRIBUTE);
        GS_ASSERT(this->group != NULL_ATTRIBUTE_GROUP);
        GS_ASSERT(this->value_type != NULL_ATTRIBUTE_VALUE_TYPE);
        GS_ASSERT(this->name[0] != '\0');
        GS_ASSERT(this->sync_type != NULL_ATTRIBUTE_SYNC_TYPE);
        GS_ASSERT(this->sync_type != ATTRIBUTE_SYNC_TYPE_PLAYER ||
                  this->sync_to != NULL_CLIENT);

        GS_ASSERT((this->location == NULL && (this->getter != NULL &&
                                               this->setter != NULL)) ||
                  (this->location != NULL && (this->getter == NULL &&
                                               this->setter == NULL)));
    }

    void verify_other(const Attribute* other) const
    {
        GS_ASSERT(this->getter == NULL || this->getter != other->getter);
        GS_ASSERT(this->setter == NULL || this->setter != other->setter);
        GS_ASSERT(this->location == NULL ||
                  this->location != other->location);
        GS_ASSERT(strcmp(this->name, other->name) != 0);
    }

    Attribute() :
        Property<AttributeType>(NULL_ATTRIBUTE),
        group(NULL_ATTRIBUTE_GROUP), value_type(NULL_ATTRIBUTE_VALUE_TYPE),
        sync_type(NULL_ATTRIBUTE_SYNC_TYPE),
        location(NULL), getter(NULL), setter(NULL),
        changed(false), sync_to(NULL_CLIENT)
    {
    }

    private:

    void _add_setter(voidFunction setter)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->setter != NULL);
        this->setter = setter;
    }

    void _add_getter(voidFunction getter)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->getter != NULL);
        this->getter = getter;
    }

    void _set_location(void* location)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->location == NULL);
        this->location = location;
    }

    void _pack_message(set_attribute_int_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_INT);
        msg->attribute_type = this->type;
        msg->value = this->get_int();
    }

    void _pack_message(set_attribute_float_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_FLOAT);
        msg->attribute_type = this->type;
        msg->value = this->get_float();
    }

    void _pack_message(set_attribute_string_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_STRING);
        msg->attribute_type = this->type;
        strncpy(msg->value, this->get_string(), STRING_ATTRIBUTE_MAX_LENGTH);
        msg->value[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    }
};


class Attributes: public Properties<Attribute, AttributeType>
{
    public:
        AttributesID id;

    void verify() const
    {
        for (size_t i=0; i<this->max; i++)
            if (this->properties[i].loaded)
                this->properties[i].verify();

        for (size_t i=0; i<this->max-1; i++)
        for (size_t j=i+1; j<this->max; j++)
            if (this->properties[i].loaded && this->properties[j].loaded)
                this->properties[i].verify_other(&this->properties[j]);
    }

    void send() const
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send();
    }

    void send_changes()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send_changes();
    }

    Attributes() :
        Properties<Attribute, AttributeType>(MAX_ATTRIBUTES)
    {}
};


class AttributesManager
{
    public:

        size_t count;
        size_t max;

        Attributes** attributes;

    Attributes* create()
    {
        IF_ASSERT(this->count >= this->max) return NULL;
        Attributes* a = new Attributes;
        a->id = (AttributesID)this->count;
        this->attributes[this->count++] = a;
        return a;
    }

    Attributes* get(AttributesID id)
    {
        IF_ASSERT(!isValid(id)) return NULL;
        return this->attributes[id];
    }

    void verify() const
    {
        for (size_t i=0; i<this->count; i++)
            this->attributes[i]->verify();
    }

    AttributesManager(size_t max) :
        count(0), max(max)
    {
        this->attributes = new Attributes*[max];
    }

    ~AttributesManager()
    {
        for (size_t i=0; i<this->count; i++)
            delete this->attributes[i];
        delete[] this->attributes;
    }
};

static AttributesManager* attributes_manager;

/* Read/Write API */

void set(AttributesID id, AttributeType type, int value)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return;
    a->set(value);
}

void set(AttributesID id, AttributeType type, float value)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return;
    a->set(value);
}

void set(AttributesID id, AttributeType type, const char* value)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return;
    a->set(value);
}

void set(AttributesID id, const char* name, int value)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return;
    a->set(value);
}

void set(AttributesID id, const char* name, float value)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return;
    a->set(value);
}

void set(AttributesID id, const char* name, const char* value)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return;
    a->set(value);
}

int get_int(AttributesID id, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return 1;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return 1;
    return a->get_int();
}

float get_float(AttributesID id, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return 1.0f;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return 1.0f;
    return a->get_float();
}

const char* get_string(AttributesID id, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return NULL;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return NULL;
    return a->get_string();
}

int get_int(AttributesID id, const char* name)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return 1;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return 1;
    return a->get_int();
}

float get_float(AttributesID id, const char* name)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return 1.0f;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return 1.0f;
    return a->get_float();
}

const char* get_string(AttributesID id, const char* name)
{
    Attributes* attributes = attributes_manager->get(id);
    IF_ASSERT(attributes == NULL) return NULL;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return NULL;
    return a->get_string();
}

/* Boilerplate */

void init()
{
    GS_ASSERT(attributes_manager == NULL);
    attributes_manager = new AttributesManager(MAX_ATTRIBUTES_IDS);
}

void teardown()
{
    if (attributes_manager != NULL) delete attributes_manager;
}

void verify()
{
    attributes_manager->verify();
}

/* Registration API */

static Attributes* current_attributes;

// value trackers to allow late-loading of defaults
static AttributeType _type = NULL_ATTRIBUTE;
static AttributeValueType _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
static int _ival;
static float _fval;
static char _sval[STRING_ATTRIBUTE_MAX_LENGTH+1];

static void _set_saved()
{
    IF_ASSERT(current_attributes == NULL) return;
    current_attributes->done_loading();
    switch (_vtype)
    {
        case ATTRIBUTE_VALUE_INT:
            set(current_attributes->id, _type, _ival);
            _ival = 0;
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case ATTRIBUTE_VALUE_FLOAT:
            set(current_attributes->id, _type, _fval);
            _fval = 0.0f;
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case ATTRIBUTE_VALUE_STRING:
            set(current_attributes->id, _type, _sval);
            _sval[0] = '\0';
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case NULL_ATTRIBUTE_VALUE_TYPE:
            GS_ASSERT(false);
            break;
    }

    _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
}

AttributesID start_registration()
{
    GS_ASSERT(current_attributes == NULL);
    current_attributes = attributes_manager->create();
    IF_ASSERT(current_attributes == NULL) return NULL_ATTRIBUTES_ID;
    return current_attributes->id;
}

void end_registration()
{
    done_loading();
    current_attributes = NULL;
}

static AttributeType def(AttributeGroup group, const char* name)
{
    IF_ASSERT(current_attributes == NULL) return NULL_ATTRIBUTE;
    if (_vtype != NULL_ATTRIBUTE_VALUE_TYPE) _set_saved();
    Attribute* a = current_attributes->get_next();
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE;
    a->group = group;
    a->set_name(name);
    return a->type;
}

AttributeType def(AttributeGroup group, const char* name, int value)
{
    AttributeType type = def(group, name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    _ival = value;
    _vtype = ATTRIBUTE_VALUE_INT;
    _type = type;
    return type;
}

AttributeType def(AttributeGroup group, const char* name, float value)
{
    AttributeType type = def(group, name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    _fval = value;
    _vtype = ATTRIBUTE_VALUE_FLOAT;
    _type = type;
    return type;
}

AttributeType def(AttributeGroup group, const char* name, const char* value)
{
    AttributeType type = def(group, name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    strncpy(_sval, value, STRING_ATTRIBUTE_MAX_LENGTH+1);
    _sval[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    _vtype = ATTRIBUTE_VALUE_STRING;
    _type = type;
    return type;
}

void set_sync_type(AttributeType type, AttributeSyncType sync_type)
{
    IF_ASSERT(current_attributes == NULL) return;
    Attribute* a = current_attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_sync_type(sync_type);
}

void set_location(AttributeType type, int* location)
{
    IF_ASSERT(current_attributes == NULL) return;
    GS_ASSERT(_vtype == ATTRIBUTE_VALUE_INT);
    Attribute* a = current_attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_location(location);
}

void set_location(AttributeType type, float* location)
{
    IF_ASSERT(current_attributes == NULL) return;
    GS_ASSERT(_vtype == ATTRIBUTE_VALUE_FLOAT);
    Attribute* a = current_attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_location(location);
}

void set_location(AttributeType type, char** location)
{
    IF_ASSERT(current_attributes == NULL) return;
    GS_ASSERT(_vtype == ATTRIBUTE_VALUE_STRING);
    Attribute* a = current_attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_location(location);
}

void done_loading()
{
    IF_ASSERT(current_attributes == NULL) return;
    if (_vtype != NULL_ATTRIBUTE_VALUE_TYPE) _set_saved();
    current_attributes->done_loading();
}

/* Network */

void init_packets()
{
    set_attribute_int_StoC::register_client_packet();
    set_attribute_float_StoC::register_client_packet();
    set_attribute_string_StoC::register_client_packet();
}

#if DC_CLIENT
inline void set_attribute_int_StoC::handle()
{
    Attributes* attributes = attributes_manager->get((AttributesID)this->attributes_id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get((AttributeType)this->attribute_type);
    IF_ASSERT(attr == NULL) return;
    attr->set((int)this->value);
}

inline void set_attribute_float_StoC::handle()
{
    Attributes* attributes = attributes_manager->get((AttributesID)this->attributes_id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get((AttributeType)this->attribute_type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}

inline void set_attribute_string_StoC::handle()
{
    Attributes* attributes = attributes_manager->get((AttributesID)this->attributes_id);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get((AttributeType)this->attribute_type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}
#endif

#if DC_SERVER
inline void set_attribute_int_StoC::handle() {}
inline void set_attribute_float_StoC::handle() {}
inline void set_attribute_string_StoC::handle() {}
#endif

}   // Attributes
