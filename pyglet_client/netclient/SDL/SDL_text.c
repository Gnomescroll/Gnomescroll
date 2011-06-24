#include "SDL_text.h"


TTF_Font *font;

char* fontpath = "media/fonts/freesansbold.ttf";

int _init_text() {
    TTF_Init();
    //atexit(TTF_Quit);


    if(!(font = TTF_OpenFont(fontpath, 20))) {
        printf("Error loading font: %s", TTF_GetError());
        return 1;
    }

    return 0;
/*
    SDL_Color clrFg = {0,0,255,0};  // Blue ("Fg" is foreground)
    SDL_Surface *sText = TTF_RenderText_Solid( font, "Courier 12", clrFg );
    SDL_Rect rcDest = {0,0,0,0};
    SDL_BlitSurface( sText,NULL, screen,&rcDest );
    SDL_FreeSurface( sText );

    return 0;
*/
}
