/*=========================================================================*\
* LuaSocket toolkit
* Networking support for the Lua language
* Diego Nehab
* 26/11/1999
*
* This library is part of an  effort to progressively increase the network
* connectivity  of  the Lua  language.  The  Lua interface  to  networking
* functions follows the Sockets API  closely, trying to simplify all tasks
* involved in setting up both  client and server connections. The provided
* IO routines, however, follow the Lua  style, being very similar  to the
* standard Lua read and write functions.
*
* RCS ID: $Id: luasocket.c,v 1.47 2005/01/02 22:51:32 diego Exp $
\*=========================================================================*/

/*=========================================================================*\
* Standard include files
\*=========================================================================*/
#include "mr.h"
#include "mr_auxlib.h"
//#include "AEEStdLib.h"//ouli brew
//#include "compat-5.1.h"
#include "mr_socket_target.h"
#include "mythroad.h"

/*=========================================================================*\
* LuaSocket includes
\*=========================================================================*/

#include "mr_auxiliar.h"
#include "mr_tcp_target.h"
//#include "mythroad_brew.h"
//#include "except.h"
//#include "timeout.h"
//#include "buffer.h"
//#include "inet.h"
//#include "tcp.h"
//#include "udp.h"
//#include "select.h"



//anyka spacial ,add this for qq initnet and quit to qqlist,qqlist hasn`t socket obj
#ifdef MR_ANYKA_MOD
int32 mr_initNetworkCBState = 0;
#endif

/*-------------------------------------------------------------------------*\
* Internal function prototypes
\*-------------------------------------------------------------------------*/
static int global_skip(mrp_State *L);
static int global_unload(mrp_State *L);
static int base_open(mrp_State *L, const char *mode);

static int sock_mr_getHost(mrp_State *L){
   int32 ret;
   char* input1= ((char*)  mr_L_checkstring(L,1));
   ret = _mr_getHost(L, input1);
   mrp_pushnumber(L,(mrp_Number)ret);
   return 1;
}



/*-------------------------------------------------------------------------*\
* Modules and functions
\*-------------------------------------------------------------------------*/
static const mr_L_reg mod[] = {
//    {"auxiliar", aux_open},
//    {"except", except_open},
//    {"timeout", tm_open},
//    {"buffer", buf_open},
//    {"inet", inet_open},
   {"tcp", mr_tcp_open},
   {"udp", mr_udp_open},
//    {"select", select_open},
    {NULL, NULL}
};

static mr_L_reg socket_func[] = {
    {"__gc",  global_unload},
    {"getHost", sock_mr_getHost},
//    {"__unload",  global_unload},
//    {"skip",      global_skip},
    {NULL,        NULL}
};



/*-------------------------------------------------------------------------*\
* Close module 
\*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*\
* Skip a few arguments
\*-------------------------------------------------------------------------*/
static int global_skip(mrp_State *L) {
    int amount = mr_L_checkint(L, 1);
    int ret = mrp_gettop(L) - amount - 1;
    return ret >= 0 ? ret : 0;
}

/*-------------------------------------------------------------------------*\
* Unloads the library
\*-------------------------------------------------------------------------*/
static int global_unload(mrp_State *L) {
    MRDBGPRINTF("Clean Network!");
    mr_closeNetwork();
    return 0;
}

/*
static int sock_getstate(mrp_State *L)
{
   p_tcp tcp = (p_tcp) toptcp(L, 1);
    mrp_pushnumber(L, tcp->state);
    return 1;
}
*/


static int32 mr_initNetworkCB(int32 result)
{
//anyka spacial ,add this for qq initnet and quit to qqlist,qqlist hasn`t socket obj
#ifdef MR_ANYKA_MOD
   if(mr_initNetworkCBState != 1)
   {
      return MR_FAILED;
   }
   mr_initNetworkCBState = 0;
#endif
  
   if (!((mr_state == MR_STATE_RUN) || (mr_state == MR_STATE_PAUSE)))
   {
      MRDBGPRINTF("VM is IDLE!");
      return MR_FAILED;
   }

   mrp_pushstring(vm_state, "socket");
   mrp_rawget(vm_state, MRP_GLOBALSINDEX);  /* get traceback function */

   //add this for qq initnet and quit to qqlist,qqlist hasn`t socket obj.
   if(!mrp_istable(vm_state, -1)){
      mrp_pop(vm_state, 1);
      MRDBGPRINTF("Socket is IDLE!");
      return MR_FAILED;
   }
   //end
      
   if (result == MR_SUCCESS){
      mrp_pushstring(vm_state, "state");
      mrp_pushnumber(vm_state, MRSOCK_CONNECTED);
      mrp_rawset(vm_state, -3);
   }else if(result == MR_FAILED){
      mrp_pushstring(vm_state, "state");
      mrp_pushnumber(vm_state, MRSOCK_ERR);
      mrp_rawset(vm_state, -3);
   }else{
      MRDBGPRINTF("initNetworkCB param err!");
   }
   mrp_pop(vm_state, 1);
   return MR_SUCCESS;
}


/*-------------------------------------------------------------------------*\
* Setup basic stuff.
\*-------------------------------------------------------------------------*/
static int base_open(mrp_State *L, const char *mode) {
   int32 ret = mr_initNetwork(mr_initNetworkCB, mode);
   
//anyka spacial ,add this for qq initnet and quit to qqlist,qqlist hasn`t socket obj
#ifdef MR_ANYKA_MOD
   mr_initNetworkCBState = 1;
#endif

   mr_L_openlib(L, "socket", socket_func, 0);
#ifdef MRP_SOCKET_DEBUG
   mrp_pushstring(L, "DEBUG");
   mrp_pushboolean(L, 1);
   mrp_rawset(L, -3);
#endif


#if 0
   //socket cann`t be release at exit
   mrp_newtable(L);
   mrp_pushstring(L, "__gc");
   mrp_pushcfunction(L, global_unload);
   mrp_rawset(L, -3);
   mrp_setmetatable (L, -2);
   //end
#endif


   /* make version string available to scripts */
   //mrp_pushstring(L, "VERSION");
   //mrp_pushstring(L, MRP_SOCKET_VERSION);
   //mrp_rawset(L, -3);

   if (ret == MR_SUCCESS){
      mrp_pushstring(L, "state");
      mrp_pushnumber(L, MRSOCK_CONNECTED);
      mrp_rawset(L, -3);
   }else if(ret == MR_FAILED){
      mrp_pushstring(L, "state");
      mrp_pushnumber(L, MRSOCK_ERR);
      mrp_rawset(L, -3);
   }else if(ret == MR_WAITING){
      mrp_pushstring(L, "state");
      mrp_pushnumber(L, MRSOCK_CONNECTING);
      mrp_rawset(L, -3);
   }else{
      mrp_pushstring(L, "mr_initNetwork return err!");
      mrp_error(L);
      return 0;
   }
   return 1;
   
}

/*-------------------------------------------------------------------------*\
* Initializes all library modules.
\*-------------------------------------------------------------------------*/
MRP_SOCKET_API int mropen_socket(mrp_State *L, const char *mode) {
    int i;
    base_open(L, mode);
    for (i = 0; mod[i].name; i++) mod[i].func(L);
    return 1;
}
