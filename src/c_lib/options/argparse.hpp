#pragma once

// convert char** to

const int ARG_STRING_MAX = 127;
const int ARG_NAME_MAX = 127;

typedef union
{
    char string_arg[ARG_STRING_MAX];
    float float_arg;
    bool bool_arg;
    int int_arg;
    unsigned int uint_arg;
} ArgValue;

int parse_args(int argc, char* argv[])
{
    int n=0;

    char argname[ARG_NAME_MAX+1] = {'\0'};
    char argstr[ARG_STRING_MAX+1] = {'\0'};

    for (int i=0; i<argc; i++)
    {
        char* str = argv[i];
        GS_ASSERT(str[0] != '\0');
        if (str[0] == '\0') continue;
        if (str[0] != '-')
        {
            printf("Invalid argument style: %s", str);
            continue;
        }

        int j=0,k=0,m=0;
        char c;
        while ((c = str[j++]) != '\0' && c == '-'); // skip all -
        while ((c = str[j++]) != '\0' && c != '=' && k < ARG_NAME_MAX)
            argname[k++] = c;

        if (k == ARG_NAME_MAX && (c != '=' || c != '\0'))
        {
            printf("Argument name is too long. Error arg: %s\n", str);
            continue;
        }

        argname[k] = '\0';

        if (c != '=')
        {
            if (!strcmp(argname, (char*)"help"))
                printf("Use: --key=value\n");
            else
                printf("Invalid argument style: %s", str);
            continue;
        }

        while ((c = str[j++]) != '\0' && m < ARG_STRING_MAX)
            argstr[m++] = c;

        if (m == ARG_STRING_MAX && c != '\0')
        {
            printf("Argument value is too long. Error arg: %s\n", str);
            continue;
        }

        argstr[m] = '\0';

        // now need to coerce argstr to correct type

        //// need lookup table mapping argname to type conversion method that converts and sets value
        //void set_option_from_str(char* name, char* val)
        //{
            //int name_index = get_option_name_index(name);
            //option_coerce_and_set[name_index](name_index, val); // array of (void)(coerce)(str*)
        //}
        //void coerce_option_bool(int key, char* val)
        //{
            //int i = atoi(val);
            //bool opt = (bool)i;
            //set_option(key, opt);
        //}
        //void set_option_bool(int key, bool opt)
        //{
            //*((bool*)(options[key])) = opt;  // array of void* 
        //}

        n++;
    }
    return n;
}
