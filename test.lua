-- test.lua
-- test gdbm binding and OO interface

-- load OO interface
dofile"gdbm.lua"

-- extend DBM "class"
function DBM:show(k)
 local v=self:fetch(k) or "(not present)"
 write(k,"\t",v," [",strlen(v),"]\n")
end

function DBM:list(title)
 title=title or ""
 write("-------------------------- { ",title,"\n")
 local k=self:firstkey() 
 while k do
  self:show(k)
  k=self:nextkey(k)
 end
 write("-------------------------- } ",title,"\n")
end

function DBM:trace(s)
 if self:errno()~=GDBM_NO_ERROR then
  write(">>> DBM:",s," :",self:strerror(),"\n")
 end
end

-- now run test
F="test.gdbm"

remove(F)

print(DBM:version())

d=DBM:new()

--d:open(F,0,GDBM_READER)
d:open(F,0,GDBM_NEWDB)
	d:trace("open")
d:list("after open")

d:insert("jan","january")
	d:trace("insert")
d:insert("feb","february")
	d:trace("insert")
d:insert("mar","march")
	d:trace("insert")
d:insert("apr","april")
	d:trace("insert")
d:insert("may","may")
	d:trace("insert")
d:insert("jun","june")
	d:trace("insert")
d:insert("jul","july")
	d:trace("insert")
d:insert("aug","august")
	d:trace("insert")
d:insert("sep","september")
	d:trace("insert")
d:insert("oct","october")
	d:trace("insert")
d:insert("nov","november")
	d:trace("insert")
d:insert("dec","december")
	d:trace("insert")

d:list("after all insertions")

d:insert("lhf","LHF\000Lua")
	d:trace("insert")
d:show("lhf")
d:show("oops")
	d:trace("show")

d:delete("may")
	d:trace("delete")
d:show("may")

d:delete("MAY")
	d:trace("delete")

d:insert("lhf","LUIZ")
	d:trace("insert")
d:replace("lhf","luiz")
	d:trace("replace")
d:show("lhf")
d:replace("XXX","luiz")
	d:trace("replace")
d:show("XXX")

a=d:proxy()
a.x=10
a.z=print
a.lhf="henrique"
print(a.x,a.y,a.z,a.lhf,a.dec)
d:show("x")
d:show("lhf")

execute("ls -l "..F)
d:delete("jan")
d:delete("fev")
d:reorganize()
	d:trace("reorg")
execute("ls -l "..F)
foreach(globals(),function (i,v) d:store(i,i) end)
execute("ls -l "..F)
d:list("with all globals")

d:close()

-- done
