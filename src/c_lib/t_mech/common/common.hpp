#pragma once

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

enum MECH_TYPE
{
	MECH_CRYSTAL,
	MECH_CROP,
	MECH_WIRE,
	MECH_SWITCH
};

enum MECH_RENDER_TYPE
{
    MECH_RENDER_TYPE_0, //X shaped
    MECH_RENDER_TYPE_1, //crop/wheat type
    MECH_RENDER_TYPE_2, //mycelium/ladder type
};