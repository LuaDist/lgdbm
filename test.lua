-- test gdbm binding

function testing(s)
 print""
 print("-------------------------------------------------------",s)
end

function show(d)
 local n=0
 for k,v in d:entries() do
  n=n+1
  print(n,k,v) 
 end
end

------------------------------------------------------------------------------
print(gdbm.version)

F="test.gdbm"
d=assert(gdbm.open(F,"n"))

------------------------------------------------------------------------------
testing"insert method"
d:insert("JAN","January")
d:insert("FEB","February")
d:insert("MAR","March")
d:insert("APR","April")
d:insert("MAY","May")
d:insert("JUN","June")

show(d)

------------------------------------------------------------------------------
testing"proxy insert"
t=d:proxy()
t.JUL="July"
t.AUG="August"
t.SEP="September"
t.OCT="October"
t.NOV="November"
t.DEC="December"

show(d)

------------------------------------------------------------------------------
testing"proxy delete"
t.JAN=nil
t.FEB=nil
t.MAR=nil
t.APR=nil
t.MAY=nil
t.JUN=nil

show(d)

------------------------------------------------------------------------------
testing"return codes"
print("JAN exists?",d:exists("JAN"))
print("DEC exists?",d:exists("DEC"))
print("replace DEC?",d:replace("DEC","Dezembro"))
print("DEC is now",t.DEC)
print("insert JUL?",d:insert("JUL","Julho"))
print("insert XXX?",d:insert("XXX","Julho"))
print("JUL is",d:fetch("JUL"))
print("XXX is",d:fetch("XXX"))
print("YYY is",d:fetch("YYY"))
print("insert XXX",d:insert("XXX","a"))
print("insert XXX",d:insert("XXX","b"))
print("XXX is",d:fetch("XXX"))
print("replace XXX",d:replace("XXX","1"))
print("replace XXX",d:replace("XXX","2"))
print("XXX is",d:fetch("XXX"))
print("delete XXX",d:delete("XXX"))
print("delete XXX",d:delete("XXX"))

testing"closing and gc"
print(d)
d:close()
print(d)
d=nil
print(d)

------------------------------------------------------------------------------
print""
print(gdbm.version)
