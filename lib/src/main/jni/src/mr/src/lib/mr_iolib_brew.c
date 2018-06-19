/*
** $Id: liolib.c,v 2.39a 2003/03/19 21:16:12 roberto Exp $
** Standard I/O (and system) library
** See Copyright Notice in lua.h
*/


#include <ctype.h>
#include <string.h>
#include "AEEStdLib.h"
#include "mythroad_brew.h"			// ouli brew
#include "mr_forvm.h"


//#define liolib_c

#include "mr.h"

#include "mr_auxlib.h"
#include "mr_lib.h"

#define FILE IFile


/*
** {======================================================
** FILE Operations
** =======================================================
*/



#define FILEHANDLE		"FILE*"

#define IO_INPUT		"_input"
#define IO_OUTPUT		"_output"


static int pushresult (lua_State *L, int i, const char *filename) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  if (i) {
    lua_pushboolean(L, 1);
    return 1;
  }
  else {
    lua_pushnil(L);
    if (filename)
      //lua_pushfstring(L, "%s: %s", filename, strerror(errno));
      lua_pushfstring(L, "file err : %s: %d", filename, 
         IFILEMGR_GetLastError(pLegendGame->pFileMgr));
    else
      //lua_pushfstring(L, "%s", strerror(errno));
      lua_pushfstring(L, "file err: %d", IFILEMGR_GetLastError(pLegendGame->pFileMgr));
    lua_pushnumber(L, IFILEMGR_GetLastError(pLegendGame->pFileMgr));
    return 3;
  }
}


static FILE **topfile (lua_State *L, int findex) {
  FILE **f = (FILE **)luaL_checkudata(L, findex, FILEHANDLE);
  if (f == NULL) luaL_argerror(L, findex, "bad file");
  return f;
}


static int io_type (lua_State *L) {
  FILE **f = (FILE **)luaL_checkudata(L, 1, FILEHANDLE);
  if (f == NULL) lua_pushnil(L);
  else if (*f == NULL)
    lua_pushliteral(L, "closed file");
  else
    lua_pushliteral(L, "file");
  return 1;
}


static FILE *tofile (lua_State *L, int findex) {
  FILE **f = topfile(L, findex);
  if (*f == NULL)
    luaL_error(L, "attempt to use a closed file");
  return *f;
}



/*
** When creating file handles, always creates a `closed' file handle
** before opening the actual file; so, if there is a memory error, the
** file is not left opened.
*/
static FILE **newfile (lua_State *L) {
  FILE **pf = (FILE **)lua_newuserdata(L, sizeof(FILE *));
  *pf = NULL;  /* file handle is currently `closed' */
  luaL_getmetatable(L, FILEHANDLE);
  lua_setmetatable(L, -2);
  return pf;
}



static int aux_close (lua_State *L) {
   FILE *f = tofile(L, 1);
   int ok = (IFILE_Release(f) == 0);
   if (ok)
     *(FILE **)lua_touserdata(L, 1) = NULL;  /* mark file as closed */
   return ok;
}


static int io_close (lua_State *L) {
  if (lua_isnone(L, 1) && lua_type(L, lua_upvalueindex(1)) == LUA_TTABLE) {
    lua_pushstring(L, IO_OUTPUT);
    lua_rawget(L, lua_upvalueindex(1));
  }
  return pushresult(L, aux_close(L), NULL);
}


static int io_gc (lua_State *L) {
  FILE **f = topfile(L, 1);
  if (*f != NULL)  /* ignore closed files */
    aux_close(L);
  return 0;
}


static int io_tostring (lua_State *L) {
  char buff[128];
  FILE **f = topfile(L, 1);
  if (*f == NULL)
    STRCPY(buff, "closed");
  else
    SPRINTF(buff, "%p", lua_touserdata(L, 1));
  lua_pushfstring(L, "file (%s)", buff);
  return 1;
}


static int io_open (lua_State *L) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  const char *filename = luaL_checkstring(L, 1);
  long mode = luaL_optlong(L, 2, 0);
  int createfile = mode & MF_CREATE;
  FILE **pf = newfile(L);

  mode = mode & 0xff;
  if (mode == MF_RDONLY)
  {
    *pf = IFILEMGR_OpenFile (pLegendGame->pFileMgr, filename, _OFM_READ);
    if ((*pf == NULL) && createfile)
    {
         *pf = IFILEMGR_OpenFile (pLegendGame->pFileMgr, filename, _OFM_CREATE);
    }
  }else if (mode == MF_RDWR)
  {
    *pf = IFILEMGR_OpenFile (pLegendGame->pFileMgr, filename, _OFM_READ);
    if ((*pf == NULL) && createfile)
    {
         *pf = IFILEMGR_OpenFile (pLegendGame->pFileMgr, filename, _OFM_READWRITE);
    }
  }
  return (*pf == NULL) ? pushresult(L, 0, filename) : 1;
}



static FILE *getiofile (lua_State *L, const char *name) {
  lua_pushstring(L, name);
  lua_rawget(L, lua_upvalueindex(1));
  return tofile(L, -1);
}

static int io_readline (lua_State *L);

/*
** {======================================================
** READ
** =======================================================
*/

static int read_chars (lua_State *L, FILE *f, size_t n) {
  size_t rlen;  /* how much to read */
  size_t nr;  /* number of chars actually read */
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  rlen = LUAL_BUFFERSIZE;  /* try to read that much each time */
  do {
    char *p = luaL_prepbuffer(&b);
    if (rlen > n) rlen = n;  /* cannot read more than asked */
    nr = IFILE_Read( f, p, rlen);
    luaL_addsize(&b, nr);
    n -= nr;  /* still have to read `n' chars */
  } while (n > 0 && nr == rlen);  /* until end of count or eof */
  luaL_pushresult(&b);  /* close buffer */
  return (n == 0 || lua_strlen(L, -1) > 0);
}


static int g_read (lua_State *L, FILE *f, int first) {
  int nargs = lua_gettop(L) - 1;
  int success;
  int n;

  luaL_checkstack(L, nargs+LUA_MINSTACK, "too many arguments");

  success = 1;
  for (n = first; nargs-- && success; n++) {
     if (lua_type(L, n) == LUA_TNUMBER) {
        size_t l = (size_t)lua_tonumber(L, n);
        success = read_chars(L, f, l);
     }
  }
  if (!success) {
    lua_pop(L, 1);  /* remove last result */
    lua_pushnil(L);  /* push nil instead */
  }
  return n - first;
}

static int io_read (lua_State *L) {
  return g_read(L, getiofile(L, IO_INPUT), 1);
}


static int f_read (lua_State *L) {
  return g_read(L, tofile(L, 1), 2);
}

/* }====================================================== */


static int g_write (lua_State *L, FILE *f, int arg) {
  int nargs = lua_gettop(L) - 1;
  int status = 1;
  for (; nargs--; arg++) {
   size_t l;
   const char *s = luaL_checklstring(L, arg, &l);
   status = status && (IFILE_Write (f, s, l) == l);
  }
  return pushresult(L, status, NULL);
}


static int io_write (lua_State *L) {
  return g_write(L, getiofile(L, IO_OUTPUT), 1);
}


static int f_write (lua_State *L) {
  return g_write(L, tofile(L, 1), 2);
}


static int f_seek (lua_State *L) {
   /*
  static const int mode[] = {SEEK_SET, SEEK_CUR, SEEK_END};
  static const char *const modenames[] = {"set", "cur", "end", NULL};
  */ //ouli brew
  FILE *f = tofile(L, 1);
  int op = luaL_optint(L, 2, 0);
  long offset = luaL_optlong(L, 3, 0);
  if (op==1)
  {
    op = 2;
  }else if (op==2)
  {
    op = 1;
  }
  //op = fseek(f, offset, mode[op]);
  op = IFILE_Seek (f, op, offset);
  if (op != SUCCESS)
    return pushresult(L, 0, NULL);  /* error */
  else {
    lua_pushnumber(L, offset);
    return 1;
  }
}

static const luaL_reg iolib[] = {
  {"close", io_close},
  {"open", io_open},
  {"read", io_read},
  {"type", io_type},
  {"write", io_write},
  {NULL, NULL}
};


static const luaL_reg flib[] = {
  {"read", f_read},
  {"seek", f_seek},
  {"write", f_write},
  {"close", io_close},
  {"__gc", io_gc},
  {"__tostring", io_tostring},
  {NULL, NULL}
};


static void createmeta (lua_State *L) {
  luaL_newmetatable(L, FILEHANDLE);  /* create new metatable for file handles */
  /* file methods */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);  /* push metatable */
  lua_rawset(L, -3);  /* metatable.__index = metatable */
  luaL_openlib(L, NULL, flib, 0);
}

/* }====================================================== */


/*
** {======================================================
** Other O.S. Operations
** =======================================================
*/

static int io_remove (lua_State *L) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  const char *filename = luaL_checkstring(L, 1);
  return pushresult(L, IFILEMGR_Remove(pLegendGame->pFileMgr,filename) == SUCCESS, filename);
}


static int io_rename (lua_State *L) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  const char *fromname = luaL_checkstring(L, 1);
  const char *toname = luaL_checkstring(L, 2);
  return pushresult(L, IFILEMGR_Rename(pLegendGame->pFileMgr,fromname, toname) == SUCCESS, fromname);
}

static int io_mkdir (lua_State *L) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  const char *filename = luaL_checkstring(L, 1);
  return pushresult(L, IFILEMGR_MkDir(pLegendGame->pFileMgr,filename) == SUCCESS, filename);
}

static int io_rmdir (lua_State *L) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  const char *filename = luaL_checkstring(L, 1);
  return pushresult(L, IFILEMGR_RmDir(pLegendGame->pFileMgr,filename) == SUCCESS, filename);
}

static int io_getuptime (lua_State *L) {
  lua_pushnumber(L, GETUPTIMEMS());
  return 1;
}

static int io_getfileinfo (lua_State *L) {
  LegendGameApp *pLegendGame = (LegendGameApp*)GETAPPINSTANCE();
  const char *filename = luaL_checkstring(L, 1);
  if (IFILEMGR_GetInfo(pLegendGame->pFileMgr, filename, &pLegendGame->mr_fileInfo)!=SUCCESS)
  {
    lua_pushnil(L);
    return 1;
  }

  if (pLegendGame->mr_fileInfo.attrib == _FA_NORMAL)
  {
    lua_pushnumber(L, MF_FILE);
    return 1;
  }

  if (pLegendGame->mr_fileInfo.attrib == _FA_DIR)
  {
    lua_pushnumber(L, MF_DIR);
    return 1;
  }

  lua_pushnumber(L, MF_INVALID);
  return 1;
}


static const luaL_reg syslib[] = {
  {"getuptime",    io_getuptime},
  {"remove",    io_remove},
  {"rename",    io_rename},
  {"mkdir",    io_mkdir},
  {"rmdir",    io_rmdir},
  {"getfileinfo",    io_getfileinfo},

  {NULL, NULL}
};

/* }====================================================== */



LUALIB_API int luaopen_io (lua_State *L) {
  luaL_openlib(L, LUA_OSLIBNAME, syslib, 0);
  createmeta(L);
  lua_pushvalue(L, -1);
  luaL_openlib(L, LUA_IOLIBNAME, iolib, 1);
  return 1;
}

