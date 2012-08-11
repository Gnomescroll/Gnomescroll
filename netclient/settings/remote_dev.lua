

package.path = "settings/?.lua;?.lua"

require("lua_library");

options = {}

-- player name --
options.name = "flarb"

-- server --

options.server = "174.37.26.118"
options.port = 0

-- display options -- 
options.fullscreen = false
options.width = 1280
options.height = 720
options.fov = 85

-- mouse --
options.sensitivity = 1000
options.camera_speed = 0.6
options.invert_mouse = false

-- hud setting --
options.hud = true
options.diagnostic_hud = false
options.fps = true
options.ping = true
options.ping_update_interval = 250
options.system_message_r = 255
options.system_message_g = 255
options.system_message_b = 50

-- sound settings --
options.sound = true
options.sfx = 100
options.music = 100

-- graphics --
options.animations = true;
options.placement_outline = false;

-- help --
options.show_tips = true;

ffi = require("ffi")
ffi.cdef[[
    void LUA_set_int_option(int option_id, int value);
    void LUA_set_bool_option(int option_id, int value);
    void LUA_set_float_option(int option_id, float value);
    void LUA_set_string_option(int option_id, char* value);
]]

for key,value in pairs(options) do 
    if(options_table[key] ~= nil) then

        local id = options_table[key].id
        if( options_table[key].type == "int") then
            ffi.C.LUA_set_int_option(id, options[key]);
        elseif( options_table[key].type == "bool") then
            ffi.C.LUA_set_bool_option(id, options[key]);
        elseif( options_table[key].type == "float") then
            ffi.C.LUA_set_float_option(id, options[key]);
        elseif( options_table[key].type == "string") then
            local str = ffi.new("char[64]");
            ffi.copy(str, options[key]);
            ffi.C.LUA_set_string_option(id, str);
        else
            print("Set Options Error: type error\n"); 
        end
    else
        print("Set Options Error: option " .. key .. " does not exist \n");
    end
end
