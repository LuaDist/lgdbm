-- gdbm.lua
-- support code for gdbm library
-- usage lua -lgdbm ...

local function so(x)
	local SOPATH= os.getenv"LUA_SOPATH" or "./"
	assert(loadlib(SOPATH.."l"..x..".so","luaopen_"..x))()
end

so"gdbm"

gdbm.version=string.gsub(gdbm.version,"\n.*version (.-). .*$","using GDBM %1")

gdbm.proxymetatable= {
__index=function (t,k) return t.gdbm:fetch(k) end,
__newindex=function (t,k,v)
  if v then t.gdbm:replace(k,v) else t.gdbm:delete(k) end
 end
}

function gdbm.proxy(t)
 local a={gdbm=t}
 setmetatable(a,gdbm.proxymetatable)
 return a
end

function gdbm.keys(d)
 return function (d,k)
   if k==nil then return d:firstkey() else return d:nextkey(k) end
 end,d
end

function gdbm.entries(d)
 return function (d,k)
   local v
   if k==nil then k=d:firstkey() else k=d:nextkey(k) end
   if k==nil then v=nil else v=d:fetch(k) end
   return k,v
 end,d
end
