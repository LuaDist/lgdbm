/*
* lgdbm.c
* gdbm interface for Lua 5.1
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 30 Oct 2010 00:02:05
* This code is hereby placed in the public domain.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gdbm.h"

#include "lua.h"
#include "lauxlib.h"

#define MYNAME		"gdbm"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / Oct 2010 / \n"
#define MYTYPE		MYNAME " handle"

static datum encode(lua_State *L, int i)
{
 datum d;
 size_t l;
 d.dptr=(char*)luaL_checklstring(L,i,&l);
 d.dsize=(int)l;
 return d;
}

static int decode(lua_State *L, datum d)
{
 if (d.dptr==NULL)
  lua_pushnil(L);
 else
 {
  lua_pushlstring(L,d.dptr,d.dsize);
  free(d.dptr);
 }
 return 1;
}

static const char* errorstring(void)
{
 if (errno!=0)
  return strerror(errno);
 else if (gdbm_errno!=0)
  return gdbm_strerror(gdbm_errno);
 else
  return "(no message)";
}

static int pushresult(lua_State *L, int rc)
{
 if (rc==0)
 {
  lua_settop(L,1);
  return 1;
 }
 else
 {
  lua_pushnil(L);
  lua_pushstring(L,errorstring());
  return 2;
 }
}

static lua_State *LL=NULL;

static void errorhandler(const char *message)
{
 luaL_error(LL,"(gdbm) %s: %s",message,errorstring());
}

static GDBM_FILE Pget(lua_State *L, int i)
{
 errno=0;
 LL=L;
 return *((void**)luaL_checkudata(L,i,MYTYPE));
}

static int Lopen(lua_State *L)			/** open(file,how) */
{
 const char* file=luaL_checkstring(L,1);
 const char* how=luaL_optstring(L,2,"r");
 int read_write;
 GDBM_FILE dbf;
 GDBM_FILE* p=lua_newuserdata(L,sizeof(GDBM_FILE));
 switch (how[0])
 {
  default:
  case 'r': read_write=GDBM_READER;  break;
  case 'w': read_write=GDBM_WRITER;  break;
  case 'c': read_write=GDBM_WRCREAT; break;
  case 'n': read_write=GDBM_NEWDB;   break;
 }
 LL=L;
 errno=0;
 dbf=gdbm_open((char*)file,0,read_write,0777,errorhandler);
 if (dbf==NULL)
 {
  lua_pushnil(L);
  lua_pushfstring(L,"cannot open %s: %s",file,errorstring());
  return 2;
 }
 else
 {
  *p=dbf;
  luaL_getmetatable(L,MYTYPE);
  lua_setmetatable(L,-2);
  return 1;
 }
}

static int Lclose(lua_State *L)			/** close(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 gdbm_close(dbf);
 lua_pushnil(L);
 lua_setmetatable(L,1);
 return 0;
}

static int Lreorganize(lua_State *L)		/** reorganize(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 int rc=gdbm_reorganize(dbf);
 return pushresult(L,rc);
}

static int Lsync(lua_State *L)			/** sync(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 gdbm_sync(dbf);
 lua_settop(L,1);
 return 1;
}

static int Lexists(lua_State *L)		/** exists(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 int rc=gdbm_exists(dbf,key);
 lua_pushboolean(L,rc);
 return 1;
}

static int Lfetch(lua_State *L)			/** fetch(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 datum dat=gdbm_fetch(dbf,key);
 return decode(L,dat);
}

static int Pstore(lua_State *L, int flags)
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 datum dat=encode(L,3);
 int rc=gdbm_store(dbf,key,dat,flags);
 return pushresult(L,rc);
}

static int Linsert(lua_State *L)		/** insert(file,key,data) */
{
 return Pstore(L,GDBM_INSERT);
}

static int Lreplace(lua_State *L)		/** replace(file,key,data) */
{
 return Pstore(L,GDBM_REPLACE);
}

static int Ldelete(lua_State *L)		/** delete(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 int rc=gdbm_delete(dbf,key);
 return pushresult(L,rc);
}

static int Lfirstkey(lua_State *L)		/** firstkey(file) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum dat=gdbm_firstkey(dbf);
 return decode(L,dat);
}

static int Lnextkey(lua_State *L)		/** nextkey(file,key) */
{
 GDBM_FILE dbf=Pget(L,1);
 datum key=encode(L,2);
 datum dat=gdbm_nextkey(dbf,key);
 return decode(L,dat);
}

static int Ltostring(lua_State *L)
{
 GDBM_FILE p=Pget(L,1);
 lua_pushfstring(L,"%s %p",MYTYPE,(void*)p);
 return 1;
}

static const luaL_Reg R[] =
{
	{ "__gc",	Lclose		},
	{ "__tostring",	Ltostring	},	/** __tostring(file) */
	{ "close",	Lclose		},
	{ "delete",	Ldelete		},
	{ "exists",	Lexists		},
	{ "fetch",	Lfetch		},
	{ "firstkey",	Lfirstkey	},
	{ "insert",	Linsert		},
	{ "nextkey",	Lnextkey	},
	{ "open",	Lopen		},
	{ "reorganize",	Lreorganize	},
	{ "replace",	Lreplace	},
	{ "sync",	Lsync		},
	{ NULL,		NULL		}
};

LUALIB_API int luaopen_gdbm(lua_State *L)
{
 luaL_newmetatable(L,MYTYPE);
 lua_setglobal(L,MYNAME);
 luaL_register(L,MYNAME,R);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_pushstring(L,gdbm_version);
 lua_concat(L,2);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 return 1;
}
