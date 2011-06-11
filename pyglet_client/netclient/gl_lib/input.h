#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_keyboard.h>

typedef struct {
  int x;
  int y;
  int dx;
  int dy;
} MouseState;

typedef struct {
  int x;
  int y;
  int button1;
  int button2;
  int button3;
  int button4;
  int button5;
} MouseEvent;


//prototypes
int _init_input();
int _set_text_enty_mode(int n);
char getUnicodeValue(SDL_keysym keysym );
int _get_key_state();
int _get_key_event();


int _key_state_callback(key_state_func user_func);
int _key_event_callback(key_event_func user_func, char key);
int _mouse_movement_callback(key_movement_func user_func, MouseState ms);
int _mouse_event_callback(mouse_movement_func user_func, MouseEvent me);
