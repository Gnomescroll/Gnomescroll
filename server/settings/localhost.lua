

package.path = "settings/?.lua;?.lua"

require("lua_library");

print("LuaJit: run_lua_test finished");

if( options_table == nil) then
    print "WTF"
end

print(table.val_to_str(options_table))

options = {}

-- server name --
options.server_name = "server_one"

-- server --
options.ip_address = "0.0.0.0"  -- use default (probably 127.0.0.1)
options.port = 0                -- use default 

-- map --
options.map = ''
options.seed = 303

-- logger --
options.logger = true
options.log_chat = false

-- game --
options.pvp = false
options.base_move_rate = 30*60*60 --1hr

-- auth --
options.auth = false

-- serializer --
options.serializer = false
options.redis_database = 2

ffi = require("ffi")
ffi.cdef[[
    void LUA_set_int_option(int option_id, int value);
    void LUA_set_uint_option(int option_id, unsigned int value);
    void LUA_set_bool_option(int option_id, int value);
    void LUA_set_float_option(int option_id, float value);
    void LUA_set_string_option(int option_id, char* value);
]]

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
