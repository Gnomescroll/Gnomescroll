#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_keyboard.h>

typedef struct {
  int x;
  int y;
  int dx;
  int dy;
} MouseState;

//prototypes
int _init_input();
int _set_text_enty_mode(int n);
char getUnicodeValue(SDL_keysym keysym );
int _get_key_state();
int _get_key_event();


int _key_state_callback(callbackfunc user_func);
int _key_event_callback(callbackfunc user_func, char key);
int _mouse_movement_callback(callbackfunc user_func);
int _mouse_event_callback(callbackfunc user_func);
