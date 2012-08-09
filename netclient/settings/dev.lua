

package.path = "settings/?.lua;?.lua"

require("lua_library");

--print("LuaJit: run_lua_test finished");

if( options_table == nil) then
    print "WTF"
end


--sprint(table.val_to_str(options_table))




options = {}


-- player name --
options.name = "flarb"

-- server --

---options.server = "174.37.26.119"
options.server = "127.0.0.1"
options.port = 0
---options.port = 4096


-- display options -- 
options.fullscreen = false
options.width = 1280
options.height = 720
options.fov = 85


--1280x720

-- mouse --
options.sensitivity = 1000
options.camera_speed = 0.6
options.invert_mouse = false

-- hud setting --
options.hud = true
options.diagnostic_hud = true
options.fps = true
options.ping = true
options.ping_update_interval = 500
options.system_message_r = 255
options.system_message_g = 255
options.system_message_b = 50

-- sound settings --
options.sound = true
options.sfx = 50
options.music = 100

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
            print("Set Options Error: type error"); 
        end
    else
        print("Set Options Error: option " .. key .. " does not exist ");
    end
end
