

package.path = "lua/?.lua;lua/block/?.lua;?.lua"

require("lua_library");

print("LuaJit: run_lua_test finished");

if( options_table == nil) then
    print "WTF\n"
else
    print "WTF2\n"
end


print(table.val_to_str(options_table))



ffi = require("ffi")
ffi.cdef[[
    void LUA_set_int_option(int option_id, int value);
    void LUA_set_bool_option(int option_id, int value);
    void LUA_set_float_option(int option_id, float value);
    void LUA_set_string_option(int option_id, char* value);
]]

options = {}


options.width = 1280
options.height = 800

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
            ffi.C.LUA_set_string_option(id, options[key]);
        else
            print("Set Options Error: type error\n"); 
        end
    else
        print("Set Options Error: option " .. key .. " does not exist \n");
    end
end
