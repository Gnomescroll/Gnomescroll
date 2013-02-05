#include "attributes.hpp"

#include <common/dat/properties.hpp>
#if DC_SERVER
# include <net_lib/server.hpp>
#endif

namespace Attributes
{

class Attributes;

class Attribute: public Property<AttributeType>
{
    public:
        Attributes* parent;
        AttributeGroup group;
        AttributeValueType value_type;
        AttributeSyncType sync_type;

        voidFunction get_callback;
        voidFunction set_callback;

        bool changed;
        ClientID sync_to;

        struct
        {
            bool lower, upper;
        } use_limits;

        struct
        {
            union
            {
                int loweri;
                float lowerf;
            };
            union
            {
                int upperi;
                float upperf;
            };
            AttributeType lower;
            AttributeType upper;
            AttributeType dest;
        } limit;

    // when

    /* Read/write API */

    int get_int() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return 1;
        int s = *(reinterpret_cast<int*>(this->value));
        if (this->get_callback != NULL)
            return reinterpret_cast<getInt>(this->get_callback)(this->group, this->type, s);
        return s;
    }

    float get_float() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return 1.0f;
        float s = *(reinterpret_cast<float*>(this->value));
        if (this->get_callback != NULL)
            return reinterpret_cast<getFloat>(this->get_callback)(this->group, this->type, s);
        return s;
    }

    const char* get_string() const
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return NULL;
        char* s = *(reinterpret_cast<char**>(this->value));
        if (this->get_callback != NULL)
            return reinterpret_cast<getString>(this->get_callback)(this->group, this->type, s);
        return s;
    }

    bool set(int value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_INT) return false;
        int old = *(reinterpret_cast<int*>(this->value));
        value = this->enforce_limit(value);
        if (this->set_callback != NULL)
        {
            value = reinterpret_cast<setInt>(this->set_callback)(this->group, this->type, old, value);
            value = this->enforce_limit(value);
        }
        if (old != value)
        {
            *(reinterpret_cast<int*>(this->value)) = value;
            this->changed = true;
            this->notify_dest();
            return true;
        }
        return false;
    }

    bool set(float value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_FLOAT) return false;
        float old = *(reinterpret_cast<float*>(this->value));
        value = this->enforce_limit(value);
        if (this->set_callback != NULL)
        {
            value = reinterpret_cast<setFloat>(this->set_callback)(this->group, this->type, old, value);
            value = this->enforce_limit(value);
        }
        if (old != value)
        {
            *(reinterpret_cast<float*>(this->value)) = value;
            this->changed = true;
            this->notify_dest();
            return true;
        }
        return false;
    }

    bool set(const char* value)
    {
        IF_ASSERT(this->value_type != ATTRIBUTE_VALUE_STRING) return false;
        IF_ASSERT(value == NULL) value = "";
        char* old = *(reinterpret_cast<char**>(this->value));
        if (this->set_callback != NULL)
        {
            value = reinterpret_cast<setString>(this->set_callback)(this->group, this->type, old, value);
            IF_ASSERT(value == NULL) value = "";
        }
        if (strcmp(old, value) != 0)
        {
            char* loc = *(reinterpret_cast<char**>(this->value));
            strncpy(loc, value, STRING_ATTRIBUTE_MAX_LENGTH);
            GS_ASSERT(loc[STRING_ATTRIBUTE_MAX_LENGTH] == '\0');
            loc[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
            this->changed = true;
            return true;
        }
        return false;
    }

    /* Registration API */

    void add_get_callback(getInt get_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_get_callback(reinterpret_cast<voidFunction>(get_callback));
    }

    void add_get_callback(getFloat get_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_get_callback(reinterpret_cast<voidFunction>(get_callback));
    }

    void add_get_callback(getString get_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_get_callback(reinterpret_cast<voidFunction>(get_callback));
    }

    void add_set_callback(setInt set_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_INT);
        this->value_type = ATTRIBUTE_VALUE_INT;
        this->_add_set_callback(reinterpret_cast<voidFunction>(set_callback));
    }

    void add_set_callback(setFloat set_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_FLOAT);
        this->value_type = ATTRIBUTE_VALUE_FLOAT;
        this->_add_set_callback(reinterpret_cast<voidFunction>(set_callback));
    }

    void add_set_callback(setString set_callback)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE ||
                  this->value_type == ATTRIBUTE_VALUE_STRING);
        this->value_type = ATTRIBUTE_VALUE_STRING;
        this->_add_set_callback(reinterpret_cast<voidFunction>(set_callback));
    }

    void set_value_type(AttributeValueType value_type)
    {
        GS_ASSERT(this->value_type == NULL_ATTRIBUTE_VALUE_TYPE);
        this->value_type = value_type;
        switch (value_type)
        {
            case ATTRIBUTE_VALUE_INT:
                this->value = calloc(1, sizeof(int));
                break;
            case ATTRIBUTE_VALUE_FLOAT:
                this->value = calloc(1, sizeof(float));
                break;
            case ATTRIBUTE_VALUE_STRING:
                this->value = calloc(STRING_ATTRIBUTE_MAX_LENGTH+1, sizeof(char));
                break;
            case NULL_ATTRIBUTE_VALUE_TYPE:
                this->value = NULL;
                GS_ASSERT(false);
                break;
        }
    }

    void set_lower_limit(int lower)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_INT);
        GS_ASSERT(!this->use_limits.lower);
        this->use_limits.lower = true;
        this->limit.loweri = lower;
    }

    void set_upper_limit(int upper)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_INT);
        GS_ASSERT(!this->use_limits.upper);
        this->use_limits.upper = true;
        this->limit.upperi = upper;
    }

    void set_lower_limit(float lower)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_FLOAT);
        GS_ASSERT(!this->use_limits.lower);
        this->use_limits.lower = true;
        this->limit.lowerf = lower;
    }

    void set_upper_limit(float upper)
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_FLOAT);
        GS_ASSERT(!this->use_limits.upper);
        this->use_limits.upper = true;
        this->limit.upperf = upper;
    }

    void set_lower_limit(AttributeType lower);
    void set_upper_limit(AttributeType upper);

    void copy_from(const Attribute* other)
    {
        IF_ASSERT(this->value_type != other->value_type) return;
        if (this->value_type == ATTRIBUTE_VALUE_INT)
            this->set(other->get_int());
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
            this->set(other->get_float());
        else
        if (this->value_type == ATTRIBUTE_VALUE_STRING)
            this->set(other->get_string());
        else
        {
            GS_ASSERT(false);
        }
    }

    /* Networking */
    #if DC_SERVER
    void set_sync_type(AttributeSyncType sync_type)
    {
        GS_ASSERT(this->sync_type == NULL_SYNC_TYPE);
        this->sync_type = sync_type;
    }

    void set_sync_to(ClientID client_id)
    {
        GS_ASSERT(this->sync_to == NULL_CLIENT);
        GS_ASSERT(isValid(client_id));
        GS_ASSERT(this->sync_type == NULL_SYNC_TYPE ||
                  this->sync_type == SYNC_TYPE_PLAYER);
        this->sync_type = SYNC_TYPE_PLAYER;
        this->sync_to = client_id;
    }

    void send_changes()
    {
        if (!this->changed) return;
        if (this->sync_type == SYNC_TYPE_PRIVATE) return;
        if (this->sync_type == SYNC_TYPE_PLAYER &&
            !NetServer::client_is_connected(this->sync_to)) return;

        #define ROUTE_MESSAGE(MSG_TYPE) \
        { do { \
            MSG_TYPE msg; \
            this->_pack_message(&msg); \
            if (this->sync_type == SYNC_TYPE_ALL) \
                msg.broadcast(); \
            else \
            if (this->sync_type == SYNC_TYPE_PLAYER) \
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
        else
        {
            GS_ASSERT(false);
        }
        #undef ROUTE_MESSAGE
        this->changed = false;
    }

    void send_to_client(ClientID client_id) const
    {
        IF_ASSERT(!isValid(client_id)) return;
        if (this->sync_type == SYNC_TYPE_PRIVATE) return;

        #define ROUTE_MESSAGE(MSG_TYPE) \
        { do { \
            MSG_TYPE msg; \
            this->_pack_message(&msg); \
            if (this->sync_type == SYNC_TYPE_ALL) \
                msg.sendToClient(client_id); \
            else \
            if (this->sync_type == SYNC_TYPE_PLAYER) \
            { \
                GS_ASSERT(this->sync_to == client_id); \
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
        else
        {
            GS_ASSERT(false);
        }
        #undef ROUTE_MESSAGE
    }
    #endif

    /* Verification */

    void verify() const
    {
        GS_ASSERT(this->type != NULL_ATTRIBUTE);
        GS_ASSERT(this->parent != NULL);
        GS_ASSERT(this->group != NULL_ATTRIBUTE_GROUP);
        GS_ASSERT(this->value_type != NULL_ATTRIBUTE_VALUE_TYPE);
        GS_ASSERT(this->name[0] != '\0');
        GS_ASSERT(this->value != NULL);
        if (this->value_type == ATTRIBUTE_VALUE_INT)
        {
            GS_ASSERT(this->use_limits.lower ||
                      (this->limit.lower == NULL_ATTRIBUTE &&
                       this->limit.loweri == 0));
            GS_ASSERT(this->use_limits.upper ||
                      (this->limit.upper == NULL_ATTRIBUTE &&
                       this->limit.upperi == 0));
            if (this->use_limits.lower &&
                this->use_limits.upper &&
                this->limit.lower == NULL_ATTRIBUTE &&
                this->limit.upper == NULL_ATTRIBUTE)
            {
                GS_ASSERT(this->limit.loweri <= this->limit.upperi);
            }
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
        {
            GS_ASSERT(this->use_limits.lower ||
                      (this->limit.lower == NULL_ATTRIBUTE &&
                       this->limit.lowerf == 0));
            GS_ASSERT(this->use_limits.upper ||
                      (this->limit.upper == NULL_ATTRIBUTE &&
                       this->limit.upperf == 0));
            if (this->use_limits.lower &&
                this->use_limits.upper &&
                this->limit.lower == NULL_ATTRIBUTE &&
                this->limit.upper == NULL_ATTRIBUTE)
            {
                GS_ASSERT(this->limit.lowerf <= this->limit.upperf);
            }
        }
        else
        if (this->value_type == ATTRIBUTE_VALUE_STRING)
        {
            GS_ASSERT(!this->use_limits.lower && !this->use_limits.upper);
            GS_ASSERT(this->limit.loweri == 0 && this->limit.upperi == 0);
            GS_ASSERT(this->limit.lowerf == 0 && this->limit.upperf == 0);
            GS_ASSERT(this->limit.lower == NULL_ATTRIBUTE &&
                      this->limit.upper == NULL_ATTRIBUTE);
        }
        if (this->use_limits.upper)
        {
            GS_ASSERT(this->limit.upper == NULL_ATTRIBUTE ||
                      this->limit.upperi == 0);
        }
        if (this->use_limits.lower)
        {
            GS_ASSERT(this->limit.lower == NULL_ATTRIBUTE ||
                      this->limit.loweri == 0);
        }
        #if DC_SERVER
        GS_ASSERT(this->sync_type != NULL_SYNC_TYPE);
        GS_ASSERT(this->sync_type != SYNC_TYPE_PLAYER ||
                  this->sync_to != NULL_CLIENT);
        #endif
    }

    void verify_other(const Attribute* other) const
    {
        GS_ASSERT(this->get_callback == NULL ||
                  this->get_callback != other->get_callback);
        GS_ASSERT(this->set_callback == NULL ||
                  this->set_callback != other->set_callback);
        GS_ASSERT(this->value != other->value);
        GS_ASSERT(strcmp(this->name, other->name) != 0);
    }

    Attribute() :
        Property<AttributeType>(NULL_ATTRIBUTE),
        parent(NULL),
        group(NULL_ATTRIBUTE_GROUP),
        value_type(NULL_ATTRIBUTE_VALUE_TYPE),
        sync_type(NULL_SYNC_TYPE),
        get_callback(NULL), set_callback(NULL),
        changed(false), sync_to(NULL_CLIENT),
        value(NULL)
    {
        memset(&this->use_limits, 0, sizeof(this->use_limits));
        memset(&this->limit, 0, sizeof(this->limit));
        this->limit.dest = NULL_ATTRIBUTE;
        this->limit.lower = NULL_ATTRIBUTE;
        this->limit.upper = NULL_ATTRIBUTE;
    }

    ~Attribute()
    {
        if (this->value != NULL) free(this->value);
    }

    private:
        void* value;

    int enforce_limit(int value);
    float enforce_limit(float value);
    void notify_dest();

    void constraints_changed()
    {
        GS_ASSERT(this->limit.lower != NULL_ATTRIBUTE ||
                  this->limit.upper != NULL_ATTRIBUTE);
        if (this->value_type == ATTRIBUTE_VALUE_INT)
            this->set(this->get_int());
        else
        if (this->value_type == ATTRIBUTE_VALUE_FLOAT)
            this->set(this->get_float());
        else
        {
            GS_ASSERT(false);
        }
    }

    void _add_set_callback(voidFunction set_callback)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->set_callback == NULL);
        this->set_callback = set_callback;
    }

    void _add_get_callback(voidFunction get_callback)
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(this->get_callback == NULL);
        this->get_callback = get_callback;
    }

    #if DC_SERVER
    void _pack_message(set_attribute_int_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_INT);
        msg->attribute_group = this->group;
        msg->attribute_type = this->type;
        msg->value = this->get_int();
    }

    void _pack_message(set_attribute_float_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_FLOAT);
        msg->attribute_group = this->group;
        msg->attribute_type = this->type;
        msg->value = this->get_float();
    }

    void _pack_message(set_attribute_string_StoC* msg) const
    {
        GS_ASSERT(this->value_type == ATTRIBUTE_VALUE_STRING);
        msg->attribute_group = this->group;
        msg->attribute_type = this->type;
        strncpy(msg->value, this->get_string(), STRING_ATTRIBUTE_MAX_LENGTH);
        msg->value[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    }
    #endif
};


class Attributes: public Properties<Attribute, AttributeType>
{
    public:
        AttributeGroup group;

    void verify() const
    {
        // attributes should be laid out continuously from 0, although properties allows otherwise
        bool loaded = this->properties[0].loaded;
        int switch_ct = 0;
        GS_ASSERT(this->index == 0 || loaded);
        for (size_t i=0; i<this->max; i++)
        {
            if (this->properties[i].loaded)
                loaded = true;
            else
            {
                if (loaded) switch_ct++;
                loaded = false;
            }
        }
        GS_ASSERT(switch_ct <= 1);

        for (size_t i=0; i<this->max; i++)
            if (this->properties[i].loaded)
                this->properties[i].verify();

        for (size_t i=0; i<this->max-1; i++)
        for (size_t j=i+1; j<this->max; j++)
            if (this->properties[i].loaded && this->properties[j].loaded)
                this->properties[i].verify_other(&this->properties[j]);
    }

    void copy_from(const Attributes* other)
    {
        IF_ASSERT(this->max != other->max) return;
        IF_ASSERT(this->index != other->index) return;
        for (size_t i=0; i<this->index; i++)
            this->properties[i].copy_from(&other->properties[i]);
    }

    #if DC_SERVER
    void set_sync_to(ClientID client_id)
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].sync_type == SYNC_TYPE_PLAYER ||
                this->properties[i].sync_type == NULL_SYNC_TYPE)
                    this->properties[i].set_sync_to(client_id);
    }

    void set_sync_type(AttributeSyncType sync_type)
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].sync_type == NULL_SYNC_TYPE)
                this->properties[i].set_sync_type(sync_type);
    }

    void send_to_client(ClientID client_id) const
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send_to_client(client_id);
    }

    void send_changes()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].send_changes();
    }
    #endif

    void done_loading()
    {
        for (size_t i=0; i<this->index; i++)
            if (this->properties[i].loaded)
                this->properties[i].group = this->group;
        Properties<Attribute, AttributeType>::done_loading();
    }

    Attributes() :
        Properties<Attribute, AttributeType>(MAX_ATTRIBUTES)
    {
        for (size_t i=0; i<this->max; i++)
            this->properties[i].parent = this;
    }
};

void Attribute::set_lower_limit(AttributeType lower)
{
    Attribute* a = this->parent->_get_any(lower);
    IF_ASSERT(a == NULL) return;
    GS_ASSERT(this->value_type == a->value_type);
    GS_ASSERT(!this->use_limits.lower);
    GS_ASSERT(this->limit.lower == NULL_ATTRIBUTE);
    GS_ASSERT(a->limit.dest == NULL_ATTRIBUTE);
    this->use_limits.lower = true;
    this->limit.lower = lower;
    a->limit.dest = this->type;
}

void Attribute::set_upper_limit(AttributeType upper)
{
    Attribute* a = this->parent->_get_any(upper);
    IF_ASSERT(a == NULL) return;
    GS_ASSERT(this->value_type == a->value_type);
    GS_ASSERT(!this->use_limits.upper);
    GS_ASSERT(this->limit.upper == NULL_ATTRIBUTE);
    GS_ASSERT(a->limit.dest == NULL_ATTRIBUTE);
    this->use_limits.upper = true;
    this->limit.upper = upper;
    a->limit.dest = this->type;
}

int Attribute::enforce_limit(int value)
{
    if (this->use_limits.lower)
    {
        int lower = 0;
        if (this->limit.lower != NULL_ATTRIBUTE)
        {
            Attribute* a = this->parent->get(this->limit.lower);
            GS_ASSERT(a != NULL);
            if (a != NULL)
                lower = a->get_int();
        }
        else
            lower = this->limit.loweri;
        value = GS_MAX(value, lower);
    }
    if (this->use_limits.upper)
    {
        int upper = 0;
        if (this->limit.upper != NULL_ATTRIBUTE)
        {
            Attribute* a = this->parent->get(this->limit.upper);
            GS_ASSERT(a != NULL);
            if (a != NULL)
                upper = a->get_int();
        }
        else
            upper = this->limit.upperi;

        value = GS_MIN(value, upper);
    }
    return value;
}

float Attribute::enforce_limit(float value)
{
    if (this->use_limits.lower)
    {
        float lower = 0;
        if (this->limit.lower != NULL_ATTRIBUTE)
        {
            Attribute* a = this->parent->get(this->limit.lower);
            GS_ASSERT(a != NULL);
            if (a != NULL)
                lower = a->get_float();
        }
        else
            lower = this->limit.lowerf;
        value = GS_MAX(value, lower);
    }
    if (this->use_limits.upper)
    {
        float upper = 0;
        if (this->limit.upper != NULL_ATTRIBUTE)
        {
            Attribute* a = this->parent->get(this->limit.upper);
            GS_ASSERT(a != NULL);
            if (a != NULL)
                upper = a->get_float();
        }
        else
            upper = this->limit.upperf;
        value = GS_MIN(value, upper);
    }
    return value;
}

void Attribute::notify_dest()
{
    if (this->limit.dest == NULL_ATTRIBUTE) return;
    Attribute* dest = this->parent->get(this->limit.dest);
    IF_ASSERT(dest == NULL) return;
    dest->constraints_changed();
}

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
        a->group = (AttributeGroup)this->count;
        this->attributes[this->count++] = a;
        return a;
    }

    Attributes* get(AttributeGroup group)
    {
        IF_ASSERT(!isValid(group)) return NULL;
        return this->attributes[group];
    }

    void verify() const
    {
        for (size_t i=0; i<this->count; i++)
            this->attributes[i]->verify();
    }

    #if DC_SERVER
    void send_changes()
    {
        for (size_t i=0; i<this->count; i++)
            this->attributes[i]->send_changes();
    }
    #endif

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

bool set(AttributeGroup group, AttributeType type, int value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, AttributeType type, float value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, AttributeType type, const char* value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, const char* name, int value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, const char* name, float value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

bool set(AttributeGroup group, const char* name, const char* value)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return false;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return false;
    return a->set(value);
}

int get_int(AttributeGroup group, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return 1;
    return a->get_int();
}

float get_float(AttributeGroup group, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1.0f;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return 1.0f;
    return a->get_float();
}

const char* get_string(AttributeGroup group, AttributeType type)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return NULL;
    Attribute* a = attributes->get(type);
    IF_ASSERT(a == NULL) return NULL;
    return a->get_string();
}

int get_int(AttributeGroup group, const char* name)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return 1;
    return a->get_int();
}

float get_float(AttributeGroup group, const char* name)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return 1.0f;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return 1.0f;
    return a->get_float();
}

const char* get_string(AttributeGroup group, const char* name)
{
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return NULL;
    Attribute* a = attributes->get(name);
    IF_ASSERT(a == NULL) return NULL;
    return a->get_string();
}

void copy_from(AttributeGroup dest_group, AttributeGroup src_group)
{   // copies attribute values from src to dest.
    // their attributes must match exactly otherwise
    Attributes* dest = attributes_manager->get(dest_group);
    IF_ASSERT(dest == NULL) return;
    Attributes* src = attributes_manager->get(src_group);
    IF_ASSERT(src == NULL) return;
    dest->copy_from(src);
}

/* Boilerplate */

void init()
{
    GS_ASSERT(attributes_manager == NULL);
    attributes_manager = new AttributesManager(MAX_ATTRIBUTE_GROUPS);
}

void teardown()
{
    if (attributes_manager != NULL) delete attributes_manager;
}

void verify()
{
    attributes_manager->verify();
}

/* Meta API */

const char* get_name(AttributeGroup group, AttributeType type)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL;
    Attribute* a = attr->get(type);
    IF_ASSERT(a == NULL) return NULL;
    return a->name;
}

AttributeType get_type(AttributeGroup group, const char* name)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE;
    Attribute* a = attr->get(name);
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE;
    return a->type;
}

size_t get_attribute_count(AttributeGroup group)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return 0;
    // Attributes are guaranteed continuous in the property array,
    // so its safe to return the index
    return attr->index;
}

AttributeValueType get_value_type(AttributeGroup group, const char* name)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    Attribute* a = attr->get(name);
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    return a->value_type;
}

AttributeValueType get_value_type(AttributeGroup group, AttributeType type)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    Attribute* a = attr->get(type);
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE_VALUE_TYPE;
    return a->value_type;
}

/* Registration API */

static Attributes* current_attributes;

// value trackers to allow late-loading of defaults
static AttributeType _type = NULL_ATTRIBUTE;
static AttributeValueType _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
static int _ival;
static float _fval;
static char _sval[STRING_ATTRIBUTE_MAX_LENGTH+1];

static void set_value_type(AttributeType type, AttributeValueType value_type)
{
    IF_ASSERT(current_attributes == NULL) return;
    Attribute* a = current_attributes->_get_any(type);
    IF_ASSERT(a == NULL) return;
    a->set_value_type(value_type);
}

static void _set_saved()
{
    IF_ASSERT(current_attributes == NULL) return;
    current_attributes->done_loading();
    switch (_vtype)
    {
        case ATTRIBUTE_VALUE_INT:
            set(current_attributes->group, _type, _ival);
            _ival = 0;
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case ATTRIBUTE_VALUE_FLOAT:
            set(current_attributes->group, _type, _fval);
            _fval = 0.0f;
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case ATTRIBUTE_VALUE_STRING:
            set(current_attributes->group, _type, _sval);
            _sval[0] = '\0';
            _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
            break;

        case NULL_ATTRIBUTE_VALUE_TYPE:
            GS_ASSERT(false);
            break;
    }

    // unset changed; it should be false at init
    Attribute* a = current_attributes->get(_type);
    a->changed = false;

    _vtype = NULL_ATTRIBUTE_VALUE_TYPE;
}

AttributeGroup start_registration()
{
    GS_ASSERT(current_attributes == NULL);
    current_attributes = attributes_manager->create();
    IF_ASSERT(current_attributes == NULL) return NULL_ATTRIBUTE_GROUP;
    return current_attributes->group;
}

void end_registration()
{
    done_loading();
    current_attributes = NULL;
}

static AttributeType def(const char* name)
{
    IF_ASSERT(current_attributes == NULL) return NULL_ATTRIBUTE;
    if (_vtype != NULL_ATTRIBUTE_VALUE_TYPE) _set_saved();
    Attribute* a = current_attributes->get_next();
    IF_ASSERT(a == NULL) return NULL_ATTRIBUTE;
    a->set_name(name);
    return a->type;
}

AttributeType def(const char* name, int value)
{
    AttributeType type = def(name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    _ival = value;
    _vtype = ATTRIBUTE_VALUE_INT;
    _type = type;
    set_value_type(_type, _vtype);
    return type;
}

AttributeType def(const char* name, float value)
{
    AttributeType type = def(name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    _fval = value;
    _vtype = ATTRIBUTE_VALUE_FLOAT;
    _type = type;
    set_value_type(_type, _vtype);
    return type;
}

AttributeType def(const char* name, const char* value)
{
    AttributeType type = def(name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return NULL_ATTRIBUTE;
    strncpy(_sval, value, STRING_ATTRIBUTE_MAX_LENGTH+1);
    _sval[STRING_ATTRIBUTE_MAX_LENGTH] = '\0';
    _vtype = ATTRIBUTE_VALUE_STRING;
    _type = type;
    set_value_type(_type, _vtype);
    return type;
}

#define UNPACK_CURRENT_ATTRIBUTE \
    IF_ASSERT(current_attributes == NULL) return; \
    Attribute* a = current_attributes->_get_any(type); \
    IF_ASSERT(a == NULL) return;

#if DC_SERVER
void set_sync_type(AttributeType type, AttributeSyncType sync_type)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_sync_type(sync_type);
}
#endif

void add_get_callback(AttributeType type, getInt cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_get_callback(cb);
}

void add_get_callback(AttributeType type, getFloat cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_get_callback(cb);
}

void add_get_callback(AttributeType type, getString cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_get_callback(cb);
}

void add_set_callback(AttributeType type, setInt cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_set_callback(cb);
}

void add_set_callback(AttributeType type, setFloat cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_set_callback(cb);
}

void add_set_callback(AttributeType type, setString cb)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->add_set_callback(cb);
}

void set_lower_limit(AttributeType type, int lower)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_lower_limit(lower);
}

void set_upper_limit(AttributeType type, int upper)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_upper_limit(upper);
}

void set_lower_limit(AttributeType type, float lower)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_lower_limit(lower);
}

void set_upper_limit(AttributeType type, float upper)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_upper_limit(upper);
}

void set_lower_limit(AttributeType type, AttributeType lower)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_lower_limit(lower);
}

void set_upper_limit(AttributeType type, AttributeType upper)
{
    UNPACK_CURRENT_ATTRIBUTE
    a->set_upper_limit(upper);
}

void set_lower_limit(AttributeType type, const char* lower)
{
    UNPACK_CURRENT_ATTRIBUTE
    Attribute* attr = current_attributes->get(lower);
    IF_ASSERT(attr == NULL) return;
    a->set_lower_limit(attr->type);
}

void set_upper_limit(AttributeType type, const char* upper)
{
    UNPACK_CURRENT_ATTRIBUTE
    Attribute* attr = current_attributes->get(upper);
    IF_ASSERT(attr == NULL) return;
    a->set_upper_limit(attr->type);
}


#undef UNPACK_CURRENT_ATTRIBUTE

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

#if DC_SERVER
void set_sync_to(AttributeGroup group, ClientID client_id)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return;
    attr->set_sync_to(client_id);
}

void set_sync_type(AttributeGroup group, AttributeSyncType sync_type)
{
    Attributes* attr = attributes_manager->get(group);
    IF_ASSERT(attr == NULL) return;
    attr->set_sync_type(sync_type);
}

void send_to_client(AttributeGroup group, ClientID client_id)
{
    Attributes* a = attributes_manager->get(group);
    IF_ASSERT(a == NULL) return;
    a->send_to_client(client_id);
}

void send_changes()
{
    attributes_manager->send_changes();
}
#endif

#if DC_CLIENT
inline void set_attribute_int_StoC::handle()
{
    AttributeGroup group = (AttributeGroup)this->attribute_group;
    AttributeType type = (AttributeType)this->attribute_type;
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get(type);
    IF_ASSERT(attr == NULL) return;
    attr->set((int)this->value);

    printf("%s set to %d\n", get_name(group, type), get_int(group, type));
}

inline void set_attribute_float_StoC::handle()
{
    AttributeGroup group = (AttributeGroup)this->attribute_group;
    AttributeType type = (AttributeType)this->attribute_type;
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get(type);
    IF_ASSERT(attr == NULL) return;
    attr->set(this->value);
}

inline void set_attribute_string_StoC::handle()
{
    AttributeGroup group = (AttributeGroup)this->attribute_group;
    AttributeType type = (AttributeType)this->attribute_type;
    Attributes* attributes = attributes_manager->get(group);
    IF_ASSERT(attributes == NULL) return;
    Attribute* attr = attributes->get(type);
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
