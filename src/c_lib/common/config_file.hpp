#pragma once

/*
    This class
*/

class ConfigFileLoader
{
    private:
        typedef enum
        {
            CONFIG_TYPE_NONE = 0,
            CONFIG_TYPE_FLOAT,
            CONFIG_TYPE_INT,
            CONFIG_TYPE_COLOR,
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

    public:

    ConfigFileLoader() :
        cvn(0)
    {
        for (int i=0; i<this->cvm; i++)
        {
            this->cva[i].type = CONFIG_TYPE_NONE;
            this->cva[i].name = NULL;
            this->cva[i].ptr  = NULL;
        }
    }

    ~ConfigFileLoader()
    {
        for (int i=0; i<this->cvn; i++)
            if (this->cva[i].name != NULL)
                free(this->cva[i].name);
    }

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
            printf("ConfigFileLoader, process_line error: scanf fail: ret= %d \n", ret);
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
            printf("ConfigFileLoader, process_line error: '%s' does not match any defined key \n", var_name);
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
                    printf("ConfigFileLoader CONFIG_TYPE_FLOAT input_line error: %s \n", input_line);
                    printf("ConfigFileLoader CONFIG_TYPE_FLOAT proces_line: var_name= %s ret= %i value= %f \n",
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
                    printf("ConfigFileLoader CONFIG_TYPE_INT input_line error: %s \n", input_line);
                    printf("ConfigFileLoader CONFIG_TYPE_INT proces_line: var_name= %s ret= %d value= %d \n",
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
                    printf("ConfigFileLoader CONFIG_TYPE_COLOR input_line error: %s \n", input_line);
                    printf("ConfigFileLoader CONFIG_TYPE_COLOR proces_line: var_name= %s ret= %d value= %d %d %d %d \n",
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
                printf("ConfigFileLoader ERROR: var_type= CONFIG_TYPE_NONE \n");
                GS_ASSERT(false);
                break;
        }
        delete[] var_name;
        delete[] rest;
    }

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

    void load_file(const char* filename)
    {
        this->load_file(filename, false);
    }

    void name_creation_check(const char* var_name)
    {
        for (int i=0; i<this->cvn; i++)
        {
            IF_ASSERT(strcmp(this->cva[i].name, var_name) == 0)
                printf("ERROR: ConfigFileLoader, set_float key= '%s' already exists \n", var_name);
        }
    }

    void set_float(const char* var_name, float* var_loc)
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

    void set_int(const char* var_name, int* var_loc)
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

    void set_color(const char* var_name, char* var_loc)
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
};
