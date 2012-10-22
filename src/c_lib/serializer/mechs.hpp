#pragma once

namespace serializer
{

class ParsedMechPaletteData
{
    public:
        bool valid;
        char name[MECH_NAME_MAX_LENGTH+1];
        MechType mech_type;

    void reset()
    {
        this->valid = false;
        memset(this->name, 0, sizeof(this->name));
        this->mech_type = NULL_MECH_TYPE;
    }

    ParsedMechPaletteData()
    {
        this->reset();
    }
};

void init_mechs();
void teardown_mechs();

bool save_mechs();
bool load_mechs();

}   // serializer
