/*
* gdm.c
* gdbm interface to Lua
* lhf@tecgraf.puc-rio.br
* 03 Aug 2001 09:03:06
* This code is hereby placed in the public domain.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gdbm.h"
#include "lua.h"

static int L_GDBM_FILE;

#define lua_tointeger(L,i) ((int)lua_tonumber(L,i))
#define lua_ngetusertag(L,i,t) lua_touserdata(L,i)

static datum encode(lua_State *L, int i)
{
 datum d;
 d.dptr=lua_tostring(L,i);
 d.dsize=lua_strlen(L,i);
 return d;
}

static void decode(lua_State *L, datum d)
{
 if (d.dptr!=NULL) lua_pushlstring(L,d.dptr,d.dsize); else lua_pushnil(L);
}

static int L_gdbm_open(lua_State *L)
{
 const char* file=lua_tostring(L,1);
 int block_size=lua_tointeger(L,2);
 int read_write=lua_tointeger(L,3);
 int mode=lua_tointeger(L,4);
 GDBM_FILE rc=gdbm_open(file,block_size,read_write,mode,lua_error);
 if (rc!=NULL) lua_pushusertag(L,rc,L_GDBM_FILE); else lua_pushnil(L);
 return 1;
}

static int L_gdbm_close(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 gdbm_close(dbf);
 return 0;
}

static int L_gdbm_reorganize(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 int rc=gdbm_reorganize(dbf);
 lua_pushnumber(L,rc);
 return 1;
}

static int L_gdbm_sync(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 gdbm_sync(dbf);
 return 0;
}

static int L_gdbm_exists(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 datum key=encode(L,2);
 int rc=gdbm_exists(dbf,key);
 lua_pushnumber(L,rc);
 return 1;
}

static int L_gdbm_fetch(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 datum key=encode(L,2);
 datum rc=gdbm_fetch(dbf,key);
 decode(L,rc);
 free(rc.dptr);
 return 1;
}

static int L_gdbm_store(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 datum key=encode(L,2);
 datum content=encode(L,3);
 int flags=lua_tointeger(L,4);
 int rc=gdbm_store(dbf,key,content,flags);
 lua_pushnumber(L,rc);
 return 1;
}

static int L_gdbm_delete(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 datum key=encode(L,2);
 int rc=gdbm_delete(dbf,key);
 lua_pushnumber(L,rc);
 return 1;
}

static int L_gdbm_firstkey(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 datum rc=gdbm_firstkey(dbf);
 decode(L,rc);
 free(rc.dptr);
 return 1;
}

static int L_gdbm_nextkey(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 datum key=encode(L,2);
 datum rc=gdbm_nextkey(dbf,key);
 decode(L,rc);
 free(rc.dptr);
 return 1;
}

static int L_gdbm_setopt(lua_State *L)
{
 GDBM_FILE dbf=lua_ngetusertag(L,1,L_GDBM_FILE);
 int optflag=lua_tointeger(L,2);
 int optval=lua_tointeger(L,3);
 int rc=gdbm_setopt(dbf,optflag,&optval,sizeof(optval));
 lua_pushnumber(L,rc);
 return 1;
}

static int L_gdbm_version(lua_State *L)
{
 lua_pushstring(L,gdbm_version);
 return 1;
}

static int L_gdbm_errno(lua_State *L)
{
 lua_pushnumber(L,gdbm_errno);
 return 1;
}

static int L_gdbm_strerror(lua_State *L)
{
 int error=lua_tointeger(L,1);
 const char* rc=gdbm_strerror(error);
 lua_pushstring(L,rc);
 return 1;
}

int luaL_gdbm_open(lua_State *L)
{
 L_GDBM_FILE=lua_newtag(L);
 lua_register(L,"gdbm_open",L_gdbm_open);
 lua_register(L,"gdbm_close",L_gdbm_close);
 lua_register(L,"gdbm_reorganize",L_gdbm_reorganize);
 lua_register(L,"gdbm_sync",L_gdbm_sync);
 lua_register(L,"gdbm_exists",L_gdbm_exists);
 lua_register(L,"gdbm_store",L_gdbm_store);
 lua_register(L,"gdbm_fetch",L_gdbm_fetch);
 lua_register(L,"gdbm_delete",L_gdbm_delete);
 lua_register(L,"gdbm_firstkey",L_gdbm_firstkey);
 lua_register(L,"gdbm_nextkey",L_gdbm_nextkey);
 lua_register(L,"gdbm_setopt",L_gdbm_setopt);
 lua_register(L,"gdbm_version",L_gdbm_version);
 lua_register(L,"gdbm_errno",L_gdbm_errno);
 lua_register(L,"gdbm_strerror",L_gdbm_strerror);
 lua_pushnumber(L,GDBM_READER);		lua_setglobal(L,"GDBM_READER");
 lua_pushnumber(L,GDBM_WRITER);		lua_setglobal(L,"GDBM_WRITER");
 lua_pushnumber(L,GDBM_WRCREAT);	lua_setglobal(L,"GDBM_WRCREAT");
 lua_pushnumber(L,GDBM_NEWDB);		lua_setglobal(L,"GDBM_NEWDB");
 lua_pushnumber(L,GDBM_INSERT);		lua_setglobal(L,"GDBM_INSERT");
 lua_pushnumber(L,GDBM_REPLACE);	lua_setglobal(L,"GDBM_REPLACE");
 lua_pushnumber(L,GDBM_NO_ERROR);	lua_setglobal(L,"GDBM_NO_ERROR");
 return 0;
}
