dofile"gdbm.lua"

local RNAME="repository.gdbm"
local r=DBM:new()
r:open(RNAME,0,GDBM_WRCREAT)
if r:errno()~=GDBM_NO_ERROR then error("cannot open "..RNAME) end
R=r:proxy()

function dofile(name)
 local c=R[name]
 if c==nil then
  c=compile(name)
  R[name]=c
 end
 return dostring(c)	-- need a "dostring" that can handle binary files
end

-- Lua 4.1
function compile(name)
 return dump(loadfile(name))
end

-- Lua 4.0
function compile(name)
 local t="repository.tmp"
 print("compiling "..name)
 execute("luac -o "..t.." "..name)
 local f=readfrom(t)
 local c=read"*a"
 remove(t)
 return c
end
