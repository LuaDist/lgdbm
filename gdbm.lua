-- gdbm.lua
-- OO interface for gdbm binding

-- "class"
DBM=settag({},newtag())

-- delegation
settagmethod(tag(DBM),"index",
	function (t,i)
	 if t==DBM then return nil else return DBM[i] end
	end 
)

function DBM:new()
 return settag({parent=DBM},tag(DBM))
end

function DBM:open(file,block_size,read_write,mode)
 self.C=gdbm_open(file,block_size,read_write,mode or 511)
end

function DBM:close()
 gdbm_close(self.C)
 self.C=nil
end

function DBM:reorganize()
 gdbm_reorganize(self.C)
end

function DBM:sync()
 gdbm_sync(self.C)
end

function DBM:exists(key)
 return gdbm_exists(self.C,key)
end

function DBM:fetch(key)
 return gdbm_fetch(self.C,key)
end

function DBM:store(key,content,flags)
 return gdbm_store(self.C,key,content,flags)
end

function DBM:insert(key,content)
 return gdbm_store(self.C,key,content,GDBM_INSERT)
end

function DBM:replace(key,content)
 return gdbm_store(self.C,key,content,GDBM_REPLACE)
end

function DBM:delete(key)
 return gdbm_delete(self.C,key)
end

function DBM:firstkey()
 return gdbm_firstkey(self.C)
end

function DBM:nextkey(key)
 return gdbm_nextkey(self.C,key)
end

function DBM:setopt(option,value)
 return gdbm_setopt(self.C,option,value)
end

function DBM:version()
 return gdbm_version()
end

function DBM:errno()
 return gdbm_errno()
end

function DBM:strerror(errno)
 return gdbm_strerror(errno or gdbm_errno())
end

function DBM:proxy()
 return settag({dbm=self},DBM.PROXY.tag)
end

-- proxy
do
 local t=newtag()
 DBM.PROXY={
  tag=t,
  get=function (t,i)
   local d=rawget(t,"dbm")
   return d:fetch(i)
  end,
  set=function (t,i,v)
   local d=rawget(t,"dbm")
   if v then d:replace(i,v) else d:delete(k) end
  end
 }
 settagmethod(t,"gettable",DBM.PROXY.get)
 settagmethod(t,"settable",DBM.PROXY.set)
end
