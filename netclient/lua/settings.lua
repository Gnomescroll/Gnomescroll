

package.path = "lua/?.lua;lua/block/?.lua;?.lua"

require("lua_library");

print("LuaJit: run_lua_test finished");

table_print(options_table);

if( options_table == nil) then
	print "WTF\n"
else
	print "WTF2\n"
end


print(table.val_to_str(options_table))


--x = to_string(options_table);
---print(x);
--x = { h={dd=6}, g = 5 }

---print(to_string(x)); 

--print(table.val_to_str(x))