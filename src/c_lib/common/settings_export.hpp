#pragma once

/*
    This class
*/

class SettingsExport
{
    private:
        typedef enum
        {
            CONFIG_TYPE_NONE = 0,
            CONFIG_TYPE_FLOAT,
            CONFIG_TYPE_INT,
            CONFIG_TYPE_COLOR,
            CONFIG_TYPE_STRING,
        } ConfigType;

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
        IF_ASSERT(ad >= max_buff) { free(buff); return NULL; } \
        offset += ad; }

    char* export_json_varlist()
    {
        const int max_buff = 16*1024;
        char* buff = (char*) malloc(max_buff);
        size_t offset = 0;
        size_t ad = 0;
        WRITE_TO_BUFFER("[")

        const char* display_element = "test_element";
        for (int i=0, j=0; i<this->cvm; i++)
        {
            if (this->cva[i].type == CONFIG_TYPE_NONE)
                continue;
            ad = 0;
            switch (cva[i].type)
            {
                case CONFIG_TYPE_FLOAT:
                    WRITE_TO_BUFFER("[\"%s\", \"%s\", \"%s\", %f]", cva[i].name, "TYPE_FLOAT", display_element, get_float(i))
                    break;

                case CONFIG_TYPE_INT:
                    WRITE_TO_BUFFER("[\"%s\", \"%s\", \"%s\", %d]", cva[i].name, "TYPE_INT", display_element, get_int(i))
                    break;

                case CONFIG_TYPE_COLOR:
                    WRITE_TO_BUFFER("[\"%s\", \"%s\", \"%s\", [%d, %d, %d]]", cva[i].name, "TYPE_COLOR", display_element, 255, 255, 255)
                    break;

                case CONFIG_TYPE_STRING:
                    WRITE_TO_BUFFER("[\"%s\", \"%s\", \"%s\", \"%s\"]", cva[i].name, "TYPE_STRING", display_element, get_string(i))
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
            ad = 0;
            if (display_element_array[i] == NULL)
                WRITE_TO_BUFFER("['%s', '%s']", cva[i].name, "None")
            else
                WRITE_TO_BUFFER("['%s', '%s']", cva[i].name, display_element_array[i])
            if (++j != this->cvn)
                WRITE_TO_BUFFER(", ")
        }
        WRITE_TO_BUFFER("]")
        return buff;
    }

    #undef WRITE_TO_BUFFER

/*
    void process_line(const char* input_line, bool silent)
    {
        if (strlen(input_line) <= 3) return;
        if (input_line[0] == '#') return;
        char* var_name = new char[256];
        char* rest = new char[256];
        float value_float = 0.0f;
        int value_int = 0;
        int value_r,value_g,value_b,value_a;
        int ret = sscanf(input_line, "%s = %s", var_name, rest);
        if (ret == -1)
        {
            printf("SettingsExport, process_line error: scanf fail: ret= %d\n", ret);
            delete[] var_name;
            delete[] rest;
            return;
        }
        int index = -1;
        for (int i=0; i<cvn; i++)
        {
            if (strcmp(cva[i].name, var_name) == 0)
            {
                index = i;
                break;
            }
        }
        if (index == -1)
        {
            printf("SettingsExport, process_line error: '%s' does not match any defined key\n", var_name);
            delete[] var_name;
            delete[] rest;
            return;
        }
        switch (cva[index].type)
        {
            case CONFIG_TYPE_FLOAT:
                ret = sscanf(input_line, "%s = %f", var_name, &value_float);
                if (ret != 2)
                {
                    printf("SettingsExport CONFIG_TYPE_FLOAT input_line error: %s\n", input_line);
                    printf("SettingsExport CONFIG_TYPE_FLOAT proces_line: var_name= %s ret= %i value= %f\n",
                           var_name, ret, value_float);
                    break;
                }
                if (*((float*)cva[index].ptr) != value_float)
                {
                    *((float*)cva[index].ptr) = value_float;
                    if (!silent)
                        printf("Set float: %s to %f\n", var_name, value_float);
                }
                break;

            case CONFIG_TYPE_INT:
                ret = sscanf(input_line, "%s = %d", var_name, &value_int);
                if (ret != 2)
                {
                    printf("SettingsExport CONFIG_TYPE_INT input_line error: %s\n", input_line);
                    printf("SettingsExport CONFIG_TYPE_INT proces_line: var_name= %s ret= %d value= %d\n",
                           var_name, ret, value_int);
                    break;
                }
                if (*((int*)cva[index].ptr) != value_int)
                {
                    *((int*)cva[index].ptr) = value_int;
                    if (!silent)
                        printf("Set int: %s to %d\n", var_name, value_int);
                }
                break;

            case CONFIG_TYPE_COLOR:
                ret = sscanf(input_line, "%s = %d %d %d %d", var_name, &value_r, &value_g,&value_b,&value_a);
                if (ret != 5)
                {
                    printf("SettingsExport CONFIG_TYPE_COLOR input_line error: %s\n", input_line);
                    printf("SettingsExport CONFIG_TYPE_COLOR proces_line: var_name= %s ret= %d value= %d %d %d %d\n",
                           var_name, ret,
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
                        printf("Set color: %s to %d %d %d %d\n", var_name,
                               value_r, value_g, value_b, value_a);
                }
                break;

            case CONFIG_TYPE_NONE:
            default:
                printf("SettingsExport ERROR: var_type= CONFIG_TYPE_NONE\n");
                GS_ASSERT(false);
                break;
        }
        delete[] var_name;
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

    bool name_in_use(const char* var_name)
    {
        for (int i=0; i<this->cvn; i++)
        {
            IF_ASSERT(strcmp(this->cva[i].name, var_name) == 0)
            {
                printf("ERROR: SettingsExport, set_float key= '%s' already exists\n", var_name);
                return true;
            }
        }
        return false;
    }

    int get_name_index(const char* var_name)
    {
        for (int i=0; i<this->cvn; i++)
            if (strcmp(this->cva[i].name, var_name) == 0)
                return i;
        printf("ERROR: SettingsExport, key= '%s' does not exist\n", var_name);
        GS_ASSERT(false);
        return -1;
    }

    /* REGISTER */
    void register_float(const char* var_name, float* var_loc)
    {
        if (this->name_in_use(var_name)) return;
        this->cva[this->cvn].name = strdup(var_name);
        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_FLOAT;
        this->cvn++;
    }

    void register_int(const char* var_name, int* var_loc)
    {
        if (this->name_in_use(var_name)) return;
        this->cva[this->cvn].name = strdup(var_name);
        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_INT;
        this->cvn++;
    }

    void register_float(const char* var_name, char* var_loc)
    {
        if (this->name_in_use(var_name)) return;
        this->cva[this->cvn].name = strdup(var_name);
        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_COLOR;
        this->cvn++;
    }

    void register_string(const char* var_name, char** var_loc)
    {
        if (this->name_in_use(var_name)) return;
        this->cva[this->cvn].name = strdup(var_name);
        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_STRING;
        this->cvn++;
    }

    /* SET */
    void set_float(const char* var_name, float var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_FLOAT || cv.ptr == NULL) return;

        *((float*)cv.ptr) = var_value;
    }

    void set_int(const char* var_name, int var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_FLOAT || cv.ptr == NULL) return;
        *((int*)cv.ptr) = var_value;
    }

    void set_color(const char* var_name, int* var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_COLOR || cv.ptr == NULL) return;
        GS_ASSERT(false);
    }

    void set_string(const char* var_name, const char* var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        IF_ASSERT(cv.type != CONFIG_TYPE_STRING || cv.ptr == NULL) return;

        if (cv.ptr != NULL)
            free(*((char**)cv.ptr));
        *((char**)cv.ptr) = strdup(var_value);
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
        GS_ASSERT(false);
        return Color(255, 255, 255);
    }

    const char* get_string(int i)
    {
        IF_ASSERT(!this->is_valid_index(i) ||
                  this->cva[i].type != CONFIG_TYPE_STRING ||
                  this->cva[i].ptr == NULL) return NULL;
        return (*(char**)this->cva[i].ptr);
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

    bool is_valid_index(int i)
    {
        return (i >= 0 && i < cvm);
    }

    void set_display_element(const char* var_name, const char* display_type)
    {
        int index = get_name_index(var_name);
        if (index == -1)
            return;
        display_element_array[index] = strdup(display_type); //copy string with malloc
    }

};

float _testfloat0;
int _testint1;
float _testfloat2;
char* _test_string1;

void setting_export_test()
{
    printf("%s:\n", __FILE__);

    class SettingsExport* se = new SettingsExport;
    se->register_float("test_float0", &_testfloat0);
    se->register_int("test_int1", &_testint1);
    se->register_float("test_float2", &_testfloat2);
    se->register_string("test_string1", &_test_string1);
    se->set_string("test_string1", "dog");
    printf("_test_string1: %s\n", _test_string1);
    printf("get::_test_string1: %s\n", se->get_string("test_string1"));
    se->set_string("test_string1", "bug");
    printf("_test_string1: %s\n", _test_string1);
    printf("get::_test_string1: %s\n", se->get_string("test_string1"));

    se->set_display_element("test_int1", "binary_button");
    se->set_display_element("test_int1", "slider");

    char* var_list = se->export_json_varlist();
    char* display_list = se->export_json_display_element();
    printf("%s\n", var_list);
    printf("%s\n", display_list);  //this if for programmatically generating elements
    free(var_list);
    free(display_list);
}
