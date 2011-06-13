#include "./texture_loader.h"

int _init_image_loader(void) {
IMG_Init(IMG_INIT_PNG); // IMG_INIT_JPG|IMG_INIT_PNG
return 0;
}

// SDL_Surface *IMG_Load(const char *file)

SDL_Surface* _load_image(char *file) {

SDL_Surface *image;
image=IMG_Load("./texture/target.png");
if(!image) {
    printf("IMG_Load: %s\n", IMG_GetError());
    // handle error
    return 0;
} else {
    return image;
    }

}
