
//#include <string.h> 

#include <mr.h>
#include <mr_auxlib.h>

#include "AEEStdLib.h"//ouli brew
#include "auxiliar.h"
#include "mr_tcp_brew.h"
#include "mythroad_brew.h"
//#include "socket.h"
//#include "inet.h"
//#include "options.h"

/*=========================================================================*\
* Internal function prototypes
\*=========================================================================*/
static int global_create(lua_State *L);
static int meth_connect(lua_State *L);
static int meth_listen(lua_State *L);
static int meth_bind(lua_State *L);
static int meth_send(lua_State *L);
static int meth_getstats(lua_State *L);
static int meth_setstats(lua_State *L);
static int meth_getsockname(lua_State *L);
static int meth_getpeername(lua_State *L);
static int meth_shutdown(lua_State *L);
static int meth_receive(lua_State *L);
static int meth_accept(lua_State *L);
static int meth_close(lua_State *L);
static int meth_setoption(lua_State *L);
static int meth_settimeout(lua_State *L);
static int meth_getinfo(lua_State *L);
static int meth_dirty(lua_State *L);

/* tcp object methods */
static luaL_reg tcp[] = {
    {"__gc",        meth_close},
    {"__tostring",  aux_tostring},
//    {"accept",      meth_accept},
//    {"bind",        meth_bind},
    {"close",       meth_close},
    {"connect",     meth_connect},
//    {"dirty",       meth_dirty},
    {"getinfo",       meth_getinfo},
//    {"getpeername", meth_getpeername},
//    {"getsockname", meth_getsockname},
//    {"getstats",    meth_getstats},
//    {"setstats",    meth_setstats},
//    {"listen",      meth_listen},
    {"receive",     meth_receive},
    {"send",        meth_send},
//    {"setfd",       meth_setfd},
//    {"setoption",   meth_setoption},
//    {"setpeername", meth_connect},
//    {"setsockname", meth_bind},
//    {"settimeout",  meth_settimeout},
//    {"shutdown",    meth_shutdown},
    {NULL,          NULL}
};

/* functions in library namespace */
static luaL_reg func[] = {
    {"tcp", global_create},
    {NULL, NULL}
};

/*-------------------------------------------------------------------------*\
* Initializes module
\*-------------------------------------------------------------------------*/
int tcp_open(lua_State *L)
{
    /* create classes */
    aux_newclass(L, "tcp{master}", tcp);
    aux_newclass(L, "tcp{client}", tcp);
    aux_newclass(L, "tcp{server}", tcp);
    /* create class groups */
    aux_add2group(L, "tcp{master}", "tcp{any}");
    aux_add2group(L, "tcp{client}", "tcp{any}");
    aux_add2group(L, "tcp{server}", "tcp{any}");
    aux_add2group(L, "tcp{client}", "tcp{client,server}");
    aux_add2group(L, "tcp{server}", "tcp{client,server}");
    /* define library functions */
    luaL_openlib(L, NULL, func, 0); 
    return 0;
}

/*=========================================================================*\
* Lua methods
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Just call buffered IO methods
\*-------------------------------------------------------------------------*/
static int meth_send(lua_State *L) {
    p_tcp tcp = (p_tcp) aux_checkclass(L, "tcp{client}", 1);
    //return buf_meth_send(L, &tcp->buf);
    int top = lua_gettop(L);
    size_t size, sent = -777;
    const char *data = luaL_checklstring(L, 2, &size);
    long start = (long) luaL_optnumber(L, 3, 1);
    long end = (long) luaL_optnumber(L, 4, -1);
    if (start < 0) start = (long) (size+start+1);
    if (end < 0) end = (long) (size+end+1);
    if (start < 1) start = (long) 1;
    if (end > (long) size) end = (long) size;
    if (start <= end) {
       if(tcp->sock)
       {
          sent = ISOCKET_Write(tcp->sock, (byte*)data+start-1, end-start+1 );
       }else
       {
          sent = 0;
       }
   }
   switch(sent)
   {
     case AEE_NET_WOULDBLOCK:
         DBGPRINTF("tcp send:block!");
         lua_pushnumber(L, 0);
         lua_pushstring(L, "tcp send:block!"); 
         lua_pushnumber(L, sent);
         break;
     case AEE_NET_ERROR:
        DBGPRINTF("tcp send:err!");
        lua_pushnil(L);
        lua_pushstring(L, "tcp send:err!"); 
        lua_pushnumber(L, sent);
        tcp->state = MRSOCK_ERR;
        break;
     case 0:
        DBGPRINTF("tcp send:close!");
        lua_pushnil(L);
        lua_pushstring(L, "tcp send:close!"); 
        lua_pushnumber(L, sent);
        tcp->state = MRSOCK_CLOSED;
        break;
     case -777:
        DBGPRINTF("tcp send:nothing need send!");
        lua_pushnumber(L, 0);
        lua_pushnil(L);
        lua_pushnil(L);
        break;
      default:
         lua_pushnumber(L, sent);
         lua_pushnil(L);
         lua_pushnil(L);
         break;
   }
   return lua_gettop(L) - top;
}

static int meth_receive(lua_State *L) {
    p_tcp tcp = (p_tcp) aux_checkclass(L, "tcp{client}", 1);
    //return buf_meth_receive(L, &tcp->buf);
    
   int top = lua_gettop(L);
   luaL_Buffer b;
   size_t got;
   
   /* initialize buffer with optional extra prefix 
   * (useful for concatenating previous partial results) */
   luaL_buffinit(L, &b);
   /* receive new patterns */


   if (!lua_isnumber(L, 2)) 
   {
       luaL_argcheck(L, 0, 2, "TCP recv:invalid param!");
   }
   if(tcp->sock)
   {
      got = ISOCKET_Read(tcp->sock, tcp->buf, (size_t) lua_tonumber(L, 2));
   }else
   {
      got = 0;
   }
   switch(got)
   {
       case AEE_NET_WOULDBLOCK:
          //DBGPRINTF("tcp recv:block!");
          luaL_pushresult(&b);
          lua_pushstring(L, "tcp recv:block!"); 
          lua_pushvalue(L, -2); 
          lua_pushnumber(L, 0);
          lua_replace(L, -4);
          break;
       case AEE_NET_ERROR:
          DBGPRINTF("tcp recv:err!");
          luaL_pushresult(&b);
          lua_pushstring(L, "tcp recv:err!"); 
          lua_pushvalue(L, -2); 
          lua_pushnil(L);
          lua_replace(L, -4);
          tcp->state = MRSOCK_ERR;
          break;
       case 0:
          DBGPRINTF("tcp recv:close!");
          luaL_pushresult(&b);
          lua_pushstring(L, "tcp recv:close!"); 
          lua_pushvalue(L, -2); 
          lua_pushnil(L);
          lua_replace(L, -4);
          tcp->state = MRSOCK_CLOSED;
          break;
        default:
           luaL_addlstring(&b, tcp->buf, got);
           luaL_pushresult(&b);
           lua_pushnil(L);
           lua_pushnil(L);
           break;
    }

   return lua_gettop(L) - top;
}

//*************************************
static void setfield (lua_State *L, const char *key, int value) {
  lua_pushstring(L, key);
  lua_pushnumber(L, value);
  lua_rawset(L, -3);
}
static int meth_getinfo(lua_State *L)
{
    p_tcp tcp = (p_tcp) aux_checkgroup(L, "tcp{any}", 1);
    lua_newtable(L);
    setfield(L, "state", tcp->state);
    //setfield(L, "ScreenH", tcp->ScrH);
    return 1;
}
//*************************************


/*-------------------------------------------------------------------------*\
* Turns a master tcp object into a client object.
\*-------------------------------------------------------------------------*/
static void Sockapp_ConnectCB(void *cxt, int err)
{
   p_tcp tcp = (p_tcp)cxt;
   //AEE_NET_SUCCESS
   //如果出错
   if (err)
   {
      DBGPRINTF("tcp connect err %d", err);
      tcp->state = MRSOCK_ERR;
   }
   //否则
   else
   {
      //读取第一个帐号的字符串长度，进入读取函数
      //pLegendGame->netSegmentLen = 1;
      //pLegendGame->remainDataLen = pLegendGame->netSegmentLen;
   
      //pLegendGame->netSubSegmentId = 0;
   
      //ISOCKET_Readable(pLegendGame->m_piSock, Ini_ReadCB, (void*)pLegendGame);
      DBGPRINTF("tcp connect success %d", err);
      tcp->state = MRSOCK_CONNECTED;
   }
}

static int meth_connect(lua_State *L)
{
    LegendGameApp *pLegendGame = (LegendGameApp *)GETAPPINSTANCE();
    INAddr nodeINAddr;
    p_tcp tcp = (p_tcp) aux_checkclass(L, "tcp{master}", 1);
    const char *address =  luaL_checkstring(L, 2);
    unsigned short port = (unsigned short) luaL_checknumber(L, 3);

    INET_ATON(address, &nodeINAddr);
    //nodeINAddr = xConvertToINAddr(TIMESERVER_HOST);
    if(AEE_NET_SUCCESS != 
      ISOCKET_Connect(tcp->sock, nodeINAddr, HTONS(port), Sockapp_ConnectCB, tcp))
   {
       lua_pushnil(L);
       lua_pushnumber(L, ISOCKET_GetLastError(tcp->sock));
       return 2;
   }
    aux_setclass(L, "tcp{client}", 1);
    tcp->state = MRSOCK_CONNECTING;
    lua_pushnumber(L, 1);
    return 1;
}

/*-------------------------------------------------------------------------*\
* Closes socket used by object 
\*-------------------------------------------------------------------------*/
static int meth_close(lua_State *L)
{
   LegendGameApp *pLegendGame = (LegendGameApp *)GETAPPINSTANCE();
    p_tcp tcp = (p_tcp) aux_checkgroup(L, "tcp{any}", 1);
    if (tcp->sock)
   {
       ISOCKET_Cancel(tcp->sock, NULL, NULL);
       ISOCKET_Release(tcp->sock);
       tcp->sock = NULL;
   }
    tcp->state = MRSOCK_CLOSED;
    lua_pushnumber(L, 1);
    return 1;
}

/*=========================================================================*\
* Library functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Initializes module 
\*-------------------------------------------------------------------------*/
static int global_create(lua_State *L)
{
   LegendGameApp *pLegendGame = (LegendGameApp *)GETAPPINSTANCE();
   
   if (pLegendGame->m_piNet)
   {
      /* try to allocate a system socket */
      ISocket*  m_piSock = INETMGR_OpenSocket(pLegendGame->m_piNet, AEE_SOCK_STREAM);
      if(m_piSock)
      {
         /* allocate tcp object */
         p_tcp tcp = (p_tcp) lua_newuserdata(L, sizeof(t_tcp));
         /* set its type as master object */
         aux_setclass(L, "tcp{master}", -1);
         /* initialize remaining structure fields */
         tcp->sock = m_piSock;
         tcp->state = MRSOCK_OPENED;
         //io_init(&tcp->io, (p_send) sock_send, (p_recv) sock_recv, 
         //        &tcp->sock);
         //buf_init(&tcp->buf, &tcp->io);
         return 1;
      } else {
          lua_pushnil(L);
          lua_pushnumber(L, INETMGR_GetLastError(pLegendGame->m_piNet) );
          return 2;
      }
   }else{
      lua_pushnil(L);
      DBGPRINTF("Net Manager is nil!");
      return 1;
   }
}
