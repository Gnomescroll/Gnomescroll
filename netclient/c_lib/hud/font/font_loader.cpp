#include "font_loader.hpp"


namespace font_loader
{

#include <stdlib.h>
#include <string.h>

#include "struct.hpp"
#include "font_loader_class.cpp"


struct _font_meta* load_font(char* filename, int size)
{
    
    struct _font_meta* meta = (struct _font_meta*) malloc(sizeof(struct _font_meta));
    memset(meta, 0, sizeof(struct _font_meta));

    class font_loader::CFontSys c(meta);

    c.GenerateFont( filename, size);

    return meta;
}

}