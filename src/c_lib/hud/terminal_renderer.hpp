#pragma once

#include <common/input_buffer.hpp>
#include <hud/font.hpp>
//#include <t_map/common/types.hpp>

namespace Hud
{

const size_t TERMINAL_CHAR_WIDTH = 8;
const size_t TERMINAL_CHAR_HEIGHT = 8;
const size_t TERMINAL_BUFFER_SIZE = TERMINAL_CHAR_WIDTH * TERMINAL_CHAR_HEIGHT;

class TerminalRenderer
{
    public:
        HudText::Text text;
        InputBuffer buffer;

    TerminalRenderer() :
        buffer(TERMINAL_BUFFER_SIZE)
    {
        text.set_text("Boom shaka laka");
        text.center();
    }

    void draw()
    {
        text.draw();
        //if (drawn_width >= this->render_width) y += line_height;
        //if (drawn_width >= this->render_width) { y += line_height; x = 0; }
    }
};

//<rdn> know that you Text will handle almost everything. you only might have to add a new draw method and fixed width to wrap on, if its not there
//<rdn> the other thing you have to know is that input needs to be routed appropriately
//<rdn> input/handlers.cpp
//<rdn> you can model the input routing off of cube selector
//<rdn> you can model the input routing off of how cube selector is routed to
//<rdn> there are a few global input states
//<rdn> agent, camera, awesomium, chat, at least
//<rdn> dont add a new one, this will be in agent mode only
//<rdn> there is a struct InputState which holds a bunch of subvariables, you should see something for cube_selector there

//<rdn> and if they are pointing at it and are in range, display the screen, activate the cursor and set input_state.terminal = true;
//<rdn> when they are not looking, dont display anything, close any active cursors, and set input_state.terminal = false;
//<rdn> in agent_key_up_handler() { if (input_state.terminal) { Hud::terminal.handle_key_press(key); } }
//<rdn> in agent_key_up_handler() { if (input_state.terminal) { Hud::terminal.handle_key_press(key); return; } }
//<rdn> actually this doesnt work
//<rdn> it will require activation and deactivation

}   // Hud
