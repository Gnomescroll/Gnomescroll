#pragma once

//instance
class MECH
{
    public:
    int id; //-1 if not in use

    int x;
    int y;
    int z;

    int mech_type;           //mech type
    int render_type;    //lookup from array
    int subtype;        //sprite
    
    float size;
    int offset;
    float rotation;

    float offset_x;
    float offset_y;
};

struct MECH_ATTRIBUTE
{
    int mech_type;
    int mech_type_class;
    int render_type;
    int sprite_index;
    bool item_drop; // indicates whether has an item drop set
};

enum MECH_CLASS
{
    MECH_NONE,
    MECH_CRYSTAL,
    MECH_CROP,
    MECH_MYCELIUM,
    MECH_WIRE,
    MECH_SWITCH
};

enum MECH_RENDER_TYPE
{
    MECH_RENDER_TYPE_0, //X shaped/crystal/
    MECH_RENDER_TYPE_1, //crop/wheat type
    MECH_RENDER_TYPE_2, //mycelium/ladder type
};
