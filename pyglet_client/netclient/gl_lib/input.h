#include <SDL.h>
#include <SDL_opengl.h>

#include <SDL_keyboard.h>

typedef struct {
  int x;
  int y;
  int dx;
  int dy;
} MouseMotion;

typedef struct {
  int x;
  int y;
  int button;
  int state;
} MouseEvent;


//prototypes
int _init_input();
int _set_text_enty_mode(int n);
char getUnicodeValue(SDL_keysym keysym );
int _get_key_state();

int _process_events();

//call backs

typedef int (*key_state_func)(int test);
typedef int (*key_event_func)(char key);
typedef int (*mouse_motion_func)(MouseMotion ms);
typedef int (*mouse_event_func)(MouseEvent me);

int _key_state_callback(key_state_func user_func);  ///no idea how to do this yet
int _key_event_callback(key_event_func user_func, char key);
int _mouse_motion_callback(mouse_motion_func user_func, MouseMotion ms);
int _mouse_event_callback(mouse_event_func user_func, MouseEvent me);
