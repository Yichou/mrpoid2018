#ifndef MR_SOCKET_H
#define MR_SOCKET_H
/*=========================================================================*\
* LuaSocket toolkit
* Networking support for the Lua language
* Diego Nehab
* 9/11/1999
*
* RCS ID: $Id: luasocket.h,v 1.19 2005/01/02 22:44:00 diego Exp $
\*=========================================================================*/
#include <lua.h>

/*-------------------------------------------------------------------------*\
* Current luasocket version
\*-------------------------------------------------------------------------*/
#define LUASOCKET_VERSION    "MythrosdSocket 1.0"
#define LUASOCKET_COPYRIGHT  "Copyright"
#define LUASOCKET_AUTHORS    " "

/*-------------------------------------------------------------------------*\
* This macro prefixes all exported API functions
\*-------------------------------------------------------------------------*/
#ifndef LUASOCKET_API
#define LUASOCKET_API extern
#endif

/*-------------------------------------------------------------------------*\
* Initializes the library.
\*-------------------------------------------------------------------------*/
LUASOCKET_API int mropen_socket(lua_State *L);

#endif /* LUASOCKET_H */
