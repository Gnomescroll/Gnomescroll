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
        union   //0
        {
            struct
            {
                unsigned char x,y,z;
                unsigned char w; //unused
            };
            unsigned int pos;
        };

        union   //4
        {
            struct
            {
                unsigned char tx,ty,tz;
                unsigned char tw; //unused

            };
            unsigned int tex;
        };

        union   //8
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };

        union   //12
        {
            unsigned char ao[4]; 
            unsigned int AO;
        };

        //16
        float lighting[2];      //sun and manmade

    };


    struct VertexBackup
    {
        union   //0
        {
            struct
            {
                unsigned char x,y,z;
                unsigned char w; //unused
            };
            unsigned int pos;
        };

        union   //4
        {
            struct
            {
                unsigned char tx,ty,tz;
                unsigned char tw; //unused

            };
            unsigned int tex;
        };

        union   //8
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };

        union   //12
        {
            unsigned char ao[4]; 
            unsigned int AO;
        };

        //16
        float lighting[2];      //sun and manmade

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