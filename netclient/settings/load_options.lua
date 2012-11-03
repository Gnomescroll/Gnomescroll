package.path = "settings/?.lua;?.lua"

require("lua_library");

ffi = require("ffi")
ffi.cdef[[
    void LUA_set_int_option(int option_id, int value);
    void LUA_set_uint_option(int option_id, unsigned int value);
    void LUA_set_bool_option(int option_id, int value);
    void LUA_set_float_option(int option_id, float value);
    void LUA_set_string_option(int option_id, char* value);
]]

function set_options(options_file)
    -- require will not reload the file, but it uses LUA_PATH
    --require(options_file)
    options = {}
    dofile("./settings/" .. options_file .. ".lua");
    for key,value in pairs(options) do 
        if(options_table[key] ~= nil) then

            local id = options_table[key].id
            if( options_table[key].type == "uint") then
                ffi.C.LUA_set_uint_option(id, options[key]);
            elseif( options_table[key].type == "int") then
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
end
