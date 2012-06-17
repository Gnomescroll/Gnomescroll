#pragma once

namespace t_map
{

    struct PositionElement
    {
        union
        {
            struct
            {
                unsigned char x,y,z;
                unsigned char w; //unused
            };
            unsigned int pos;
        };
    };

    struct TextureElement
    {
        union
        {
            struct
            {
                unsigned char tx,ty,tz;
                unsigned char tw; //unused
            };
            unsigned int tex;
        };
    };

    struct ColorElement
    {
        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };
    };

    struct AOElement
    {
        union
        {
            struct
            {
                unsigned char ao[4]; //32
            };
            unsigned int AO;
        };
    };

    struct NormalElement
    {
        union
        {
            char normal[4]; //16
            unsigned int n;
        };
    };
    
    /*
        Replace x,y,z and tx,ty,tz by char
        to reduce size from x 36 to 16 bytes per vertex for normal blocks
    */

    /*
        Steves computer requires 4 byte aligned data
    */
    struct Vertex 
    {
        union
        {
            struct
            {
                unsigned char x,y,z;
                unsigned char w; //unused
            };
            unsigned int pos;
        };

        union
        {
            struct
            {
                unsigned char tx,ty,tz;
                unsigned char tw; //unused

            };
            unsigned int tex;
        };

        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };

        union
        {
            unsigned char ao[4]; 
            unsigned int AO;
        };

        union
        {
            unsigned char light_value[4]; 
            unsigned int lighting;
        };

    };
    
}

/*
    struct Vertex {
        float x,y,z; //12
        float tx,ty,tz; //24

        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };

        union
        {
            char normal[4]; //16
            unsigned int n;
        };

        union
        {
            unsigned char ao[4]; //32
            unsigned int AO;
        };
    };
*/