/*
* lgdbm.c
* gdbm interface for Lua 5.0
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 19 Jul 2006 14:22:44
* This code is hereby placed in the public domain.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gdbm.h"

#include "lua.h"
#include "lauxlib.h"

#ifndef lua_boxpointer
#define lua_boxpointer(L,u) \
	(*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i)	(*(void **)(lua_touserdata(L, i)))
#endif

#define MYNAME		"gdbm"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / Jul 2006 / \n"
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
 if (rc>=0)
 {
  lua_pushboolean(L,!rc);
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
 if (luaL_checkudata(L,i,MYTYPE)==NULL) luaL_typerror(L,i,MYTYPE);
 return lua_unboxpointer(L,i);
}

static GDBM_FILE Dget(lua_State *L, int i)
{
 GDBM_FILE dbf=Pget(L,i);
 if (dbf==NULL) luaL_error(L,"invalid %s",MYTYPE);
 return dbf;
}

static int Lopen(lua_State *L)			/** open(file,how) */
{
 const char* file=luaL_checkstring(L,1);
 const char* how=luaL_checkstring(L,2);
 int read_write;
 GDBM_FILE dbf;
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
  lua_boxpointer(L,dbf);
  luaL_getmetatable(L,MYTYPE);
  lua_setmetatable(L,-2);
  return 1;
 }
}

static int Lclose(lua_State *L)			/** close(file) */
{
 GDBM_FILE dbf=Dget(L,1);
 GDBM_FILE *p=lua_touserdata(L,1);
 gdbm_close(dbf);
 *p=NULL;
 return 0;
}

static int Lreorganize(lua_State *L)		/** reorganize(file) */
{
 GDBM_FILE dbf=Dget(L,1);
 int rc=gdbm_reorganize(dbf);
 return pushresult(L,rc);
}

static int Lsync(lua_State *L)			/** sync(file) */
{
 GDBM_FILE dbf=Dget(L,1);
 gdbm_sync(dbf);
 return 0;
}

static int Lexists(lua_State *L)		/** exists(file,key) */
{
 GDBM_FILE dbf=Dget(L,1);
 datum key=encode(L,2);
 int rc=gdbm_exists(dbf,key);
 lua_pushboolean(L,rc);
 return 1;
}

static int Lfetch(lua_State *L)			/** fetch(file,key) */
{
 GDBM_FILE dbf=Dget(L,1);
 datum key=encode(L,2);
 datum dat=gdbm_fetch(dbf,key);
 return decode(L,dat);
}

static int Pstore(lua_State *L, int flags)
{
 GDBM_FILE dbf=Dget(L,1);
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
 GDBM_FILE dbf=Dget(L,1);
 datum key=encode(L,2);
 int rc=gdbm_delete(dbf,key);
 return pushresult(L,rc);
}

static int Lfirstkey(lua_State *L)		/** firstkey(file) */
{
 GDBM_FILE dbf=Dget(L,1);
 datum dat=gdbm_firstkey(dbf);
 return decode(L,dat);
}

static int Lnextkey(lua_State *L)		/** nextkey(file,key) */
{
 GDBM_FILE dbf=Dget(L,1);
 datum key=encode(L,2);
 datum dat=gdbm_nextkey(dbf,key);
 return decode(L,dat);
}

static int Lfdesc(lua_State *L)			/** fdesc(file) */
{
#ifdef GDBM_NOLOCK
 GDBM_FILE dbf=Dget(L,1);
 lua_pushnumber(L,gdbm_fdesc(dbf));
#else
 lua_pushnil(L);
#endif
 return 1;
}

static int Ltostring(lua_State *L)              /** tostring(file) */
{
 GDBM_FILE p=Pget(L,1);
 char s[64];
 sprintf(s,"%s %p (%p)",MYTYPE,(void*)p,lua_touserdata(L,1));
 lua_pushstring(L,s);
 return 1;
}

static int Lgc(lua_State *L)
{
 GDBM_FILE dbf=Pget(L,1);
 if (dbf!=NULL) gdbm_close(dbf);
 return 0;
}

static const luaL_reg R[] =
{
	{ "__gc",	Lgc		},
	{ "__tostring",	Ltostring	},
	{ "close",	Lclose		},
	{ "delete",	Ldelete		},
	{ "exists",	Lexists		},
	{ "fdesc",	Lfdesc		},
	{ "fetch",	Lfetch		},
	{ "firstkey",	Lfirstkey	},
	{ "insert",	Linsert		},
	{ "nextkey",	Lnextkey	},
	{ "open",	Lopen		},
	{ "reorganize",	Lreorganize	},
	{ "replace",	Lreplace	},
	{ "sync",	Lsync		},
	{ "tostring",	Ltostring	},
	{ NULL,		NULL		}
};

LUALIB_API int luaopen_gdbm(lua_State *L)
{
 lua_pushliteral(L,MYNAME);
 luaL_newmetatable(L,MYTYPE);
 luaL_openlib(L,NULL,R,0);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_pushstring(L,gdbm_version);
 lua_concat(L,2);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 lua_rawset(L,LUA_GLOBALSINDEX);
 return 1;
}
