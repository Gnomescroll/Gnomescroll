#pragma once

typedef enum
{
    CONFIG_TYPE_NONE = 0,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_COLOR,
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_BOOL,
} ConfigType;

class SettingsExport
{
    private:
        struct ConfigValue
        {
            ConfigType type;
            char* name;
            void* ptr;      //address to writeback to
        };

        static const int cvm = 128;
        struct ConfigValue cva[cvm];
        int cvn;
        char* display_element_array[cvm];

    public:

    SettingsExport() :
        cvn(0)
    {
        for (int i=0; i<cvm; i++)
        {
            this->cva[i].type = CONFIG_TYPE_NONE;
            this->cva[i].name = NULL;
            this->cva[i].ptr = NULL;
        }
        for (int i=0; i<cvm; i++)
            display_element_array[i] = NULL;
    }

    ~SettingsExport()
    {
        for (int i=0; i<cvm; i++)
        {
            free(this->cva[i].name);
            if (this->cva[i].type == CONFIG_TYPE_STRING &&
                this->cva[i].ptr != NULL)
            {
                free(*((char**)this->cva[i].ptr));
            }
        }
        for (int i=0; i<cvm; i++)
            free(display_element_array[i]);
    }

    #define WRITE_TO_BUFFER(...) { \
        ad = snprintf(buff+offset, max_buff, __VA_ARGS__); \
        IF_ASSERT(ad >= (size_t) max_buff) { free(buff); return NULL; } \
        offset += ad; }

    #define ITEM_FMT(ITEM) "[\"%s\", \"%s\", \"%s\", " ITEM "]"
    #define DISPLAY_ELEMENT_FMT "[\"%s\", \"%s\"]"

    char* export_json_varlist()
    {
        const int max_buff = 16*1024;
        char* buff = (char*) malloc(max_buff);
        size_t offset = 0;
        size_t ad = 0;
        WRITE_TO_BUFFER("[")

        const char* display_element = "test_element";
        Color color = Color(0);
        for (int i=0, j=0; i<this->cvm; i++)
        {
            if (this->cva[i].type == CONFIG_TYPE_NONE)
                continue;
            ad = 0;
            switch (cva[i].type)
            {
                case CONFIG_TYPE_FLOAT:
                    WRITE_TO_BUFFER(ITEM_FMT("%f"), cva[i].name,
                                    "TYPE_FLOAT", display_element, get_float(i))
                    break;

                case CONFIG_TYPE_INT:
                    WRITE_TO_BUFFER(ITEM_FMT("%d"), cva[i].name,
                                    "TYPE_INT", display_element, get_int(i))
                    break;

                case CONFIG_TYPE_COLOR:
                    color = get_color(i);
                    WRITE_TO_BUFFER(ITEM_FMT("[%d, %d, %d]"),
                                    cva[i].name, "TYPE_COLOR", display_element,
                                    color.r, color.g, color.b)
                    break;

                case CONFIG_TYPE_STRING:
                    WRITE_TO_BUFFER(ITEM_FMT("\"%s\""), cva[i].name,
                                    "TYPE_STRING", display_element, get_string(i))
                    break;

                case CONFIG_TYPE_BOOL:
                    WRITE_TO_BUFFER(ITEM_FMT("%s"), cva[i].name, "TYPE_BOOL",
                                    display_element, get_bool(i) ? "true" : "false");
                    break;

                case CONFIG_TYPE_NONE:
                default:
                    printf("SettingsExport ERROR: var_type= CONFIG_TYPE_NONE\n");
                    GS_ASSERT(false);
                    break;
            }
            if (++j != this->cvn)
                WRITE_TO_BUFFER(", ")
        }
        WRITE_TO_BUFFER("]");
        return buff;
    }

    char* export_json_display_element()
    {
        const int max_buff = 16*1024;
        char* buff = (char*) malloc(max_buff);
        size_t offset = 0;
        size_t ad = 0;
        WRITE_TO_BUFFER("[")

        for (int i=0, j=0; i<this->cvm; i++)
        {
            if (this->cva[i].type == CONFIG_TYPE_NONE)
                continue;
            if (display_element_array[i] == NULL)
                WRITE_TO_BUFFER(DISPLAY_ELEMENT_FMT, cva[i].name, "None")
            else
                WRITE_TO_BUFFER(DISPLAY_ELEMENT_FMT, cva[i].name, display_element_array[i])
            if (++j != this->cvn)
                WRITE_TO_BUFFER(", ")
        }
        WRITE_TO_BUFFER("]")
        return buff;
    }

    #undef ITEM_PREFIX
    #undef WRITE_TO_BUFFER

/*
    void process_line(const char* input_line, bool silent)
    {
        if (strlen(input_line) <= 3) return;
        if (input_line[0] == '#') return;
        char* name = new char[256];
        char* rest = new char[256];
        float value_float = 0.0f;
        int value_int = 0;
        int value_r,value_g,value_b,value_a;
        int ret = sscanf(input_line, "%s = %s", name, rest);
        if (ret == -1)
        {
            printf("SettingsExport, process_line error: scanf fail: ret= %d\n", ret);
            delete[] name;
            delete[] rest;
            return;
        }
        int index = -1;
        for (int i=0; i<cvn; i++)
        {
            if (strcmp(cva[i].name, name) == 0)
            {
                index = i;
                break;
            }
        }
        if (index == -1)
        {
            printf("SettingsExport, process_line error: '%s' does not match any defined key\n", name);
            delete[] name;
            delete[] rest;
            return;
        }
        switch (cva[index].type)
        {
            case CONFIG_TYPE_FLOAT:
                ret = sscanf(input_line, "%s = %f", name, &value_float);
                if (ret != 2)
                {
                    printf("SettingsExport CONFIG_TYPE_FLOAT input_line error: %s\n", input_line);
                    printf("SettingsExport CONFIG_TYPE_FLOAT proces_line: name= %s ret= %i value= %f\n",
                           name, ret, value_float);
                    break;
                }
                if (*((float*)cva[index].ptr) != value_float)
                {
                    *((float*)cva[index].ptr) = value_float;
                    if (!silent)
                        printf("Set float: %s to %f\n", name, value_float);
                }
                break;

            case CONFIG_TYPE_INT:
                ret = sscanf(input_line, "%s = %d", name, &value_int);
                if (ret != 2)
                {
                    printf("SettingsExport CONFIG_TYPE_INT input_line error: %s\n", input_line);
                    printf("SettingsExport CONFIG_TYPE_INT proces_line: name= %s ret= %d value= %d\n",
                           name, ret, value_int);
                    break;
                }
                if (*((int*)cva[index].ptr) != value_int)
                {
                    *((int*)cva[index].ptr) = value_int;
                    if (!silent)
                        printf("Set int: %s to %d\n", name, value_int);
                }
                break;

            case CONFIG_TYPE_COLOR:
                ret = sscanf(input_line, "%s = %d %d %d %d", name, &value_r, &value_g,&value_b,&value_a);
                if (ret != 5)
                {
                    printf("SettingsExport CONFIG_TYPE_COLOR input_line error: %s\n", input_line);
                    printf("SettingsExport CONFIG_TYPE_COLOR proces_line: name= %s ret= %d value= %d %d %d %d\n",
                           name, ret,
                           value_r,value_g,value_b,value_a);
                    break;
                }
                ((unsigned char*)(&value_int))[0] = value_r;
                ((unsigned char*)(&value_int))[1] = value_g;
                ((unsigned char*)(&value_int))[2] = value_b;
                ((unsigned char*)(&value_int))[3] = value_a;
                if (*((int*)cva[index].ptr) != value_int)
                {
                    *((int*)cva[index].ptr) = value_int;
                    if (!silent)
                        printf("Set color: %s to %d %d %d %d\n", name,
                               value_r, value_g, value_b, value_a);
                }
                break;

            case CONFIG_TYPE_NONE:
            default:
                printf("SettingsExport ERROR: var_type= CONFIG_TYPE_NONE\n");
                GS_ASSERT(false);
                break;
        }
        delete[] name;
        delete[] rest;
    }
*/

/*
    void load_file(const char* filename, bool silent)
    {   //silent for avoiding print
        size_t buffer_size = 0;
        char* buffer = read_file_to_buffer(filename, &buffer_size);
        IF_ASSERT(buffer == NULL) return;
        size_t marker = 0;
        size_t offset = 0;
        char* tmp_str = new char[1024];
        while (marker < buffer_size)
        {
            GS_ASSERT(marker < buffer_size);
            if (buffer[marker] != '\n')
            {
                marker++;
                continue;
            }
            for (int i=0; i<1024; i++)
                tmp_str[i] = '\0';
            memcpy(tmp_str, buffer+offset, marker - offset);
            tmp_str[marker - offset] = '\0';
            process_line(tmp_str, silent);
            marker++;
            offset = marker;
        }
        free(buffer);
        delete[] tmp_str;
    }
*/

/*
    void load_file(const char* filename)
    {
        this->load_file(filename, false);
    }
*/

    bool name_in_use(const char* name)
    {
        for (int i=0; i<this->cvn; i++)
        {
            IF_ASSERT(strcmp(this->cva[i].name, name) == 0)
            {
                printf("ERROR: SettingsExport, set_float key= '%s' already exists\n", name);
                return true;
            }
        }
        return false;
    }

    int get_name_index(const char* name)
    {
        for (int i=0; i<this->cvn; i++)
            if (strcmp(this->cva[i].name, name) == 0)
                return i;
        printf("ERROR: SettingsExport, key= '%s' does not exist\n", name);
        GS_ASSERT(false);
        return -1;
    }

    bool is_valid_index(int i)
    {
        return (i >= 0 && i < cvm);
    }

    bool full()
    {
        return (this->cvn >= this->cvm);
    }

    /* REGISTER */
    int _track(const char* name, void* loc, ConfigType type)
    {
        IF_ASSERT(this->full()) return -1;
        this->cva[this->cvn].name = strdup(name);
        this->cva[this->cvn].ptr = loc;
        this->cva[this->cvn].type = type;
        return this->cvn++;
    }

    int track(const char* name, float* loc)
    {
        if (this->name_in_use(name)) return -1;
        return this->_track(name, loc, CONFIG_TYPE_FLOAT);
    }

    int track(const char* name, int* loc)
    {
        if (this->name_in_use(name)) return -1;
        return this->_track(name, loc, CONFIG_TYPE_INT);
    }

    int track(const char* name, Color* loc)
    {
        if (this->name_in_use(name)) return -1;
        return this->_track(name, loc, CONFIG_TYPE_COLOR);
    }

    int track(const char* name, char** loc)
    {
        if (this->name_in_use(name)) return -1;
        return this->_track(name, loc, CONFIG_TYPE_STRING);
    }

    int track(const char* name, bool* loc)
    {
        if (this->name_in_use(name)) return -1;
        return this->_track(name, loc, CONFIG_TYPE_BOOL);
    }

    /* SET */
    void set(const char* name, float val)
    {
        int index = get_name_index(name);
        IF_ASSERT(!this->is_valid_index(index)) return;
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_FLOAT || cv.ptr == NULL) return;
        *((float*)cv.ptr) = val;
    }

    void set(const char* name, int val)
    {
        int index = get_name_index(name);
        IF_ASSERT(!this->is_valid_index(index)) return;
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_INT || cv.ptr == NULL) return;
        *((int*)cv.ptr) = val;
    }

    void set(const char* name, const Color& val)
    {
        int index = get_name_index(name);
        IF_ASSERT(!this->is_valid_index(index)) return;
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_COLOR || cv.ptr == NULL) return;
        (*(Color*)cv.ptr) = val;
    }

    void set(const char* name, const char* val)
    {
        int index = get_name_index(name);
        IF_ASSERT(!this->is_valid_index(index)) return;
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_STRING || cv.ptr == NULL) return;
        if (cv.ptr != NULL)
            free(*((char**)cv.ptr));
        *((char**)cv.ptr) = strdup(val);
    }

    void set(const char* name, bool val)
    {
        int index = get_name_index(name);
        IF_ASSERT(!this->is_valid_index(index)) return;
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_BOOL || cv.ptr == NULL) return;
        *((bool*)cv.ptr) = val;
    }

    /* GET */
    float get_float(int i)
    {
        IF_ASSERT(!this->is_valid_index(i) ||
                  this->cva[i].type != CONFIG_TYPE_FLOAT ||
                  this->cva[i].ptr == NULL) return 0.0f;
        return *((float*)this->cva[i].ptr);
    }

    int get_int(int i)
    {
        IF_ASSERT(!this->is_valid_index(i) ||
                  this->cva[i].type != CONFIG_TYPE_INT ||
                  this->cva[i].ptr == NULL) return 0;
        return *((int*)this->cva[i].ptr);
    }

    Color get_color(int i)
    {
        IF_ASSERT(!this->is_valid_index(i) ||
                  this->cva[i].type != CONFIG_TYPE_COLOR ||
                  this->cva[i].ptr == NULL) return Color(0, 0, 0);
        return *((Color*)this->cva[i].ptr);
    }

    const char* get_string(int i)
    {
        IF_ASSERT(!this->is_valid_index(i) ||
                  this->cva[i].type != CONFIG_TYPE_STRING ||
                  this->cva[i].ptr == NULL) return NULL;
        return (*(char**)this->cva[i].ptr);
    }

    bool get_bool(int i)
    {
        IF_ASSERT(!this->is_valid_index(i) ||
                  this->cva[i].type != CONFIG_TYPE_BOOL ||
                  this->cva[i].ptr == NULL) return NULL;
        return (*(bool*)this->cva[i].ptr);
    }

    float get_float(const char* name)
    {
        float i = this->get_name_index(name);
        return this->get_float(i);
    }

    int get_int(const char* name)
    {
        int i = this->get_name_index(name);
        return this->get_int(i);
    }

    Color get_color(const char* name)
    {
        int i = this->get_name_index(name);
        return this->get_color(i);
    }

    const char* get_string(const char* name)
    {
        int i = this->get_name_index(name);
        return this->get_string(i);
    }

    bool get_bool(const char* name)
    {
        int i = this->get_name_index(name);
        return this->get_string(i);
    }

    void set_display_element(const char* name, const char* display_type)
    {
        int index = get_name_index(name);
        if (!this->is_valid_index(index)) return;
        display_element_array[index] = strdup(display_type);
    }

    ConfigType get_config_type(const char* name)
    {
        int index = this->get_name_index(name);
        if (!this->is_valid_index(index)) return CONFIG_TYPE_NONE;
        return this->cva[index].type;
    }
};

float _testfloat0 = 3.335f;
int _testint1 = 2;
float _testfloat2 = 1.0f;
char* _test_string1 = NULL;
bool _test_bool1 = true;
Color _test_color = Color(66, 77, 99);

#define SETTINGS_EXPORT_TEST 0

void setting_export_test()
{
    #if SETTINGS_EXPORT_TEST
    printf("%s:\n", __FILE__);

    class SettingsExport* se = new SettingsExport;
    se->track("test_float0", &_testfloat0);
    se->track("test_int1", &_testint1);
    se->track("test_float2", &_testfloat2);
    se->track("test_string1", &_test_string1);
    se->track("test_bool1", &_test_bool1);
    se->track("test_color1", &_test_color);
    se->set("test_string1", "dog");
    printf("_test_string1: %s\n", _test_string1);
    printf("get::_test_string1: %s\n", se->get_string("test_string1"));
    se->set("test_string1", "bug");
    printf("_test_string1: %s\n", _test_string1);
    printf("get::_test_string1: %s\n", se->get_string("test_string1"));

    se->set_display_element("test_int1", "binary_button");
    se->set_display_element("test_int1", "slider");
    delete se;
    #endif
}

#undef SETTINGS_EXPORT_TEST
