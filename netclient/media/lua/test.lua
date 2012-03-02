-- Test FFI
local ffi = require("ffi")
ffi.cdef[[
int barfunc(int foo);
]]
local barreturn = ffi.C.barfunc(253)
io.write(barreturn)
io.write('\n')
