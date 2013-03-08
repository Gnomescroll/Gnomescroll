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
            this->cva[i].ptr  = NULL;
        }

        for (int i=0; i<cvm; i++)
        {
            display_element_array[i]  = NULL;
        }

    }

    ~SettingsExport()
    {
        for (int i=0; i<cvm; i++)
            if (this->cva[i].name != NULL)
                free(this->cva[i].name);

        for(int i=0; i<cvm; i++)
        {
            if(display_element_array[i] != NULL)
                free(display_element_array[i]);
        }
    }

    char* export_json_varlist()
    {
        const int max_buff = 16*1024;
        char* buff = (char*) malloc(max_buff);
        int offset = 0;
        int _ad;
        _ad = sprintf(buff+offset, "[\n");
        offset += _ad;

        const char* display_element = "test_element";
        for (int i=0; i<this->cvm; i++)
        {
            if(this->cva[i].type == CONFIG_TYPE_NONE)
                continue;
            _ad = 0;
            switch (cva[i].type)
            {
                case CONFIG_TYPE_FLOAT:
                    _ad = sprintf(buff+offset, "\t['%s', '%s', '%s', %f],\n", cva[i].name, "TYPE_FLOAT", display_element, get_float(cva[i]) );
                    break;

                case CONFIG_TYPE_INT:
                    _ad = sprintf(buff+offset, "\t['%s', '%s', '%s', %d],\n", cva[i].name, "TYPE_INT", display_element, get_int(cva[i]));
                    break;

                case CONFIG_TYPE_COLOR:
                    _ad = sprintf(buff+offset, "\t['%s', '%s', '%s', [%d, %d, %d]],\n", cva[i].name, "TYPE_COLOR", display_element, 255, 255, 255);
                    break;

                case CONFIG_TYPE_STRING:
                    _ad = sprintf(buff+offset, "\t['%s', '%s', '%s', '%s'],\n", cva[i].name, "TYPE_STRING", display_element, get_string(cva[i]));
                    GS_ASSERT(false);
                    break;

                case CONFIG_TYPE_NONE:
                default:
                    printf("SettingsExport ERROR: var_type= CONFIG_TYPE_NONE \n");
                    GS_ASSERT(false);
                    break;
            }
            GS_ASSERT(_ad > 0);
            offset += _ad;
        }
        _ad = sprintf(buff+offset, "]\n");
        offset += _ad;
        buff[offset] = 0x00;        

        return buff;
    }


    char* export_json_display_element()
    {
        const int max_buff = 16*1024;
        char* buff = (char*) malloc(max_buff);
        int offset = 0;
        int _ad;
        _ad = sprintf(buff+offset, "[\n");
        offset += _ad;

        for (int i=0; i<this->cvm; i++)
        {
            if(this->cva[i].type == CONFIG_TYPE_NONE)
                continue;
            _ad = 0;
            if( display_element_array[i] == NULL)
            {
                _ad = sprintf(buff+offset, "\t['%s', '%s'],\n", cva[i].name, "None"); //default element
                offset  += _ad;
            }
            else
            {
                _ad = sprintf(buff+offset, "\t['%s', '%s'],\n", cva[i].name, display_element_array[i]);
                offset  += _ad;
            }

        }
        _ad = sprintf(buff+offset, "]\n");
        offset += _ad;
        buff[offset] = 0x00;        

        return buff; 
    }
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
            printf("SettingsExport, process_line error: scanf fail: ret= %d \n", ret);
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
            printf("SettingsExport, process_line error: '%s' does not match any defined key \n", var_name);
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
                    printf("SettingsExport CONFIG_TYPE_FLOAT input_line error: %s \n", input_line);
                    printf("SettingsExport CONFIG_TYPE_FLOAT proces_line: var_name= %s ret= %i value= %f \n",
                           var_name, ret, value_float);
                    break;
                }
                if (*((float*)cva[index].ptr) != value_float)
                {
                    *((float*)cva[index].ptr) = value_float;
                    if (!silent)
                        printf("Set float: %s to %f \n", var_name, value_float);
                }
                break;

            case CONFIG_TYPE_INT:
                ret = sscanf(input_line, "%s = %d", var_name, &value_int);
                if (ret != 2)
                {
                    printf("SettingsExport CONFIG_TYPE_INT input_line error: %s \n", input_line);
                    printf("SettingsExport CONFIG_TYPE_INT proces_line: var_name= %s ret= %d value= %d \n",
                           var_name, ret, value_int);
                    break;
                }
                if (*((int*)cva[index].ptr) != value_int)
                {
                    *((int*)cva[index].ptr) = value_int;
                    if (!silent)
                        printf("Set int: %s to %d \n", var_name, value_int);
                }
                break;

            case CONFIG_TYPE_COLOR:
                ret = sscanf(input_line, "%s = %d %d %d %d", var_name, &value_r, &value_g,&value_b,&value_a);
                if (ret != 5)
                {
                    printf("SettingsExport CONFIG_TYPE_COLOR input_line error: %s \n", input_line);
                    printf("SettingsExport CONFIG_TYPE_COLOR proces_line: var_name= %s ret= %d value= %d %d %d %d \n",
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
                        printf("Set color: %s to %d %d %d %d \n", var_name,
                               value_r, value_g, value_b, value_a);
                }
                break;

            case CONFIG_TYPE_NONE:
            default:
                printf("SettingsExport ERROR: var_type= CONFIG_TYPE_NONE \n");
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
                tmp_str[i] = 0x00;
            memcpy(tmp_str, buffer+offset, marker - offset);
            tmp_str[marker - offset] = 0x00;
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

    void name_creation_check(const char* var_name)
    {
        for (int i=0; i<this->cvn; i++)
        {
            IF_ASSERT(strcmp(this->cva[i].name, var_name) == 0)
                printf("ERROR: SettingsExport, set_float key= '%s' already exists \n", var_name);
        }
    }

    int get_name_index(const char* var_name)
    {
        for (int i=0; i<this->cvn; i++)
        {
            if(strcmp(this->cva[i].name, var_name) == 0)
                return i;
        }
        printf("ERROR: SettingsExport, key= '%s' does not exists \n", var_name);
        GS_ASSERT(false);
        return -1;
    }

    /* REGISTER */
    void register_float(const char* var_name, float* var_loc)
    {
        this->name_creation_check(var_name);
        //this->cva[this->cvn].name = new char[strlen(var_name)+1];
        //strcpy(this->cva[this->cvn].name, var_name);
        //this->cva[this->cvn].name[strlen(var_name)] = '\0';
        cva[this->cvn].name = strdup(var_name);
        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_FLOAT;
        this->cvn++;
    }

    void register_int(const char* var_name, int* var_loc)
    {
        this->name_creation_check(var_name);
        //this->cva[this->cvn].name = new char[strlen(var_name)+1];
        //strcpy(this->cva[this->cvn].name, var_name);
        //this->cva[this->cvn].name[strlen(var_name)] = '\0';
        cva[this->cvn].name = strdup(var_name);
        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_INT;
        this->cvn++;
    }

    void register_float(const char* var_name, char* var_loc)
    {
        this->name_creation_check(var_name);
        //this->cva[this->cvn].name = new char[strlen(var_name)+1];
        //strcpy(this->cva[this->cvn].name, var_name);
        //this->cva[this->cvn].name[strlen(var_name)] = '\0';
        cva[this->cvn].name = strdup(var_name);

        this->cva[this->cvn].ptr = var_loc;
        this->cva[this->cvn].type = CONFIG_TYPE_COLOR;
        this->cvn++;
    }

    void register_string(const char* var_name, char** var_loc) 
    {

    }




    /* SET */
    void set_float(const char* var_name, float var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        GS_ASSERT(cv.type == CONFIG_TYPE_FLOAT && cv.ptr != NULL);

        *((float*)cv.ptr) = var_value;
    }

    void set_int(const char* var_name, int var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        GS_ASSERT(cv.type == CONFIG_TYPE_FLOAT && cv.ptr != NULL);

        *((int*) cv.ptr) = var_value;
    }

    void set_color(const char* var_name, int* var_value)
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        GS_ASSERT(cv.type == CONFIG_TYPE_COLOR && cv.ptr != NULL);
    
        GS_ASSERT(false);
        return;
    }

    void set_string(const char* var_name, const char* var_value) 
    {
        int index = get_name_index(var_name);
        struct ConfigValue cv = cva[index];
        GS_ASSERT(cv.type == CONFIG_TYPE_STRING && cv.ptr != NULL);

        if(cv.ptr != NULL)
            free(cv.ptr);
        cv.ptr = strdup(var_value);
    }


/*
            CONFIG_TYPE_FLOAT,
            CONFIG_TYPE_INT,
            CONFIG_TYPE_COLOR,
            CONFIG_TYPE_STRING,
*/
    /* GET */
    float get_float(struct ConfigValue cv)
    {   
        GS_ASSERT(cv.type == CONFIG_TYPE_FLOAT && cv.ptr != NULL);
        return *((float*)cv.ptr);
    }

    int get_int(struct ConfigValue cv)
    {
        GS_ASSERT(cv.type == CONFIG_TYPE_INT && cv.ptr != NULL);
        return *((int*)cv.ptr);
    }

    void get_color(struct ConfigValue cv)
    {
        GS_ASSERT(cv.type == CONFIG_TYPE_COLOR && cv.ptr != NULL);
        GS_ASSERT(false);
        return;
    }
    
    char* get_string(struct ConfigValue cv)
    {
        GS_ASSERT(cv.type == CONFIG_TYPE_STRING && cv.ptr != NULL);
        return ((char*)cv.ptr);
    }


    void set_display_element(char* var_name, const char* display_type)
    {
        int index = get_name_index(var_name);
        if(index == -1)
            return;
        display_element_array[index] = strdup(display_type); //copy string with malloc
    }

};

float _testfloat0;
int _testint1;
float _testfloat2;

void setting_export_test()
{
    printf("common/settings_export.hpp: \n");
    
    class SettingsExport* SE = new SettingsExport;
    SE->register_float("test_float0", &_testfloat0);
    SE->register_int("test_int1", &_testint1);
    SE->register_float("test_float2", &_testfloat2);

    SE->set_display_element("test_int1", "binary_button");
    SE->set_display_element("test_int1", "slider");

    printf("%s\n", SE->export_json_varlist());
    printf("%s\n", SE->export_json_display_element());

    //abort();
}
