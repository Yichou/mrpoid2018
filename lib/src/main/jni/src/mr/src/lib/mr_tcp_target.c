
//#include <string.h> 

#include "mr.h"
#include "mr_auxlib.h"

//#include "AEEStdLib.h"//ouli brew
#include "mr_auxiliar.h"
#include "mr_tcp_target.h"
#include "mr_socket_target.h"
//#include "socket.h"
//#include "inet.h"
//#include "options.h"
#include "mrporting.h"


#define TCPHANDLE		"tcp"
#define UDPHANDLE		"udp"


/*=========================================================================*\
* Internal function prototypes
\*=========================================================================*/
static int global_create(mrp_State *L);
static int global_udp_create(mrp_State *L);
static int meth_connect(mrp_State *L);
static int meth_listen(mrp_State *L);
static int meth_bind(mrp_State *L);
static int meth_send(mrp_State *L);
static int meth_sendto(mrp_State *L);
static int meth_getstats(mrp_State *L);
static int meth_setstats(mrp_State *L);
static int meth_getsockname(mrp_State *L);
static int meth_getpeername(mrp_State *L);
static int meth_shutdown(mrp_State *L);
static int meth_receive(mrp_State *L);
static int meth_receivefrom(mrp_State *L);
static int meth_accept(mrp_State *L);
static int meth_close(mrp_State *L);
static int meth_setoption(mrp_State *L);
static int meth_settimeout(mrp_State *L);
static int meth_getinfo(mrp_State *L);
static int meth_getstate(mrp_State *L);
static int meth_dirty(mrp_State *L);
static int meth_getsock(mrp_State *L);

/* tcp object methods */
static mr_L_reg tcp[] = {
    {"__gc",        meth_close},
    {"__str",  mr_aux_tostring},
//    {"accept",      meth_accept},
//    {"bind",        meth_bind},
    {"close",       meth_close},
    {"connect",     meth_connect},
//    {"dirty",       meth_dirty},
    {"getinfo",       meth_getinfo},
    {"getstate",       meth_getstate},
    {"getsock",       meth_getsock},
    {"bind", meth_bind},
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
static mr_L_reg tcp_func[] = {
    {"tcp", global_create},
    {NULL, NULL}
};



/* udp object methods */
static mr_L_reg udp[] = {
    {"__gc",        meth_close},
    {"__str",  mr_aux_tostring},
    {"close",       meth_close},
//    {"dirty",       meth_dirty},
    {"getsock",       meth_getsock},
//    {"getpeername", meth_getpeername},
    {"bind", meth_bind},
    {"receive",     meth_receive},
    {"receivefrom", meth_receivefrom},
    {"send",        meth_send},
    {"sendto",      meth_sendto},
    {"getinfo",       meth_getinfo},
    {"getstate",       meth_getstate},
    {"connect",     meth_connect},
//    {"setfd",       meth_setfd},
//    {"setoption",   meth_setoption},
//    {"setpeername", meth_setpeername},
//    {"setsockname", meth_setsockname},
//    {"settimeout",  meth_settimeout},
    {NULL,          NULL}
};

/* functions in library namespace */
static mr_L_reg udp_func[] = {
    {"udp", global_udp_create},
    {NULL, NULL}
};



static p_tcp toptcp (mrp_State *L, int findex) {
  p_tcp f = (p_tcp)mr_L_checkudata(L, findex, TCPHANDLE);
  if (f == NULL) f = (p_tcp)mr_L_checkudata(L, findex, UDPHANDLE);
  if (f == NULL) mr_L_argerror(L, findex, "bad tcp/udp");
  return f;
}


/*-------------------------------------------------------------------------*\
* Initializes module
\*-------------------------------------------------------------------------*/
int mr_tcp_open(mrp_State *L)
{
#if 0
    /* create classes */
    mr_aux_newclass(L, "tcp{master}", tcp);
    mr_aux_newclass(L, "tcp{client}", tcp);
    mr_aux_newclass(L, "tcp{server}", tcp);
    /* create class groups */
    mr_aux_add2group(L, "tcp{master}", "tcp{any}");
    mr_aux_add2group(L, "tcp{client}", "tcp{any}");
    mr_aux_add2group(L, "tcp{server}", "tcp{any}");
    //aux_add2group(L, "tcp{client}", "tcp{client,server}");
    //aux_add2group(L, "tcp{server}", "tcp{client,server}");
    /* define library functions */
#endif
    mr_L_newmetatable(L, TCPHANDLE);  /* create new metatable for file handles */
    /* file methods */
    mrp_pushliteral(L, "__index");
    mrp_pushvalue(L, -2);  /* push metatable */
    mrp_rawset(L, -3);  /* metatable.__index = metatable */

    mr_L_openlib(L, NULL, tcp, 0);
    mrp_pop(L, 1);
    
    mr_L_openlib(L, NULL, tcp_func, 0); 
    return 0;
}

int mr_udp_open(mrp_State *L)
{
    mr_L_newmetatable(L, UDPHANDLE);  /* create new metatable for file handles */
    /* file methods */
    mrp_pushliteral(L, "__index");
    mrp_pushvalue(L, -2);  /* push metatable */
    mrp_rawset(L, -3);  /* metatable.__index = metatable */

    mr_L_openlib(L, NULL, udp, 0);
    mrp_pop(L, 1);
    
    mr_L_openlib(L, NULL, udp_func, 0); 
    return 0;
}


/*=========================================================================*\
* Lua methods
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Just call buffered IO methods
\*-------------------------------------------------------------------------*/
static int meth_send(mrp_State *L) {
   p_tcp tcp = (p_tcp) toptcp(L, 1);
   //int top = mrp_gettop(L);
   int32 size, sent = MR_FAILED;
   const char *data = mr_L_checklstring(L, 2, (size_t*)&size);
   long start = (long) mr_L_optnumber(L, 3, 1);
   long end = (long) mr_L_optnumber(L, 4, -1);

   
   //MRDBGPRINTF("send01, %d", tcp->sock);
   if (start < 0) start = (long) (size+start+1);
   if (end < 0) end = (long) (size+end+1);
   if (start < 1) start = (long) 1;
   if (end > (long) size) end = (long) size;
   
   //MRDBGPRINTF("before send,start = %d, end = %d, size= %d", start, end, size);
   if (start <= end) {
      //MRDBGPRINTF("send02, %d", tcp->sock);
      if(tcp->sock!=MR_FAILED){
         sent = mr_send(tcp->sock, (const char*)data+start-1, end-start+1 );
      }else{
         sent = MR_FAILED;
      }
      if( sent >=0 ){
         mrp_pushnumber(L, sent);
         mrp_pushnil(L);
         mrp_pushnil(L);
      }else{
         mrp_pushnil(L);
         mrp_pushstring(L, "tcp send:err!"); 
         mrp_pushnumber(L, sent);
         tcp->state = MRSOCK_ERR;
         //MRDBGPRINTF("send, error");
      }
   }else{
       mrp_pushnumber(L, 0);
       mrp_pushnil(L);
       mrp_pushnil(L);
   }
   //MRDBGPRINTF("after send,sent = %d", sent);
   return 3;
}

static int meth_sendto(mrp_State *L) {
   p_tcp tcp = (p_tcp) toptcp(L, 1);
   //int top = mrp_gettop(L);
   int32 size, sent = MR_FAILED;
   const char *data = mr_L_checklstring(L, 2, (size_t*)&size);
   int32 ip =  mr_L_checknumber(L, 3);
   uint16 port = (uint16) mr_L_checknumber(L, 4);
   long start = (long) mr_L_optnumber(L, 5, 1);
   long end = (long) mr_L_optnumber(L, 6, -1);

   
   if (start < 0) start = (long) (size+start+1);
   if (end < 0) end = (long) (size+end+1);
   if (start < 1) start = (long) 1;
   if (end > (long) size) end = (long) size;
   
   //MRDBGPRINTF("before send,start = %d, end = %d, size= %d", start, end, size);
   if (start <= end) {
      if(tcp->sock!=MR_FAILED){
         sent = mr_sendto(tcp->sock, (const char*)data+start-1, end-start+1, ip, port);
      }else{
         sent = MR_FAILED;
      }
      if( sent >=0 ){
         mrp_pushnumber(L, sent);
         mrp_pushnil(L);
         mrp_pushnil(L);
      }else{
         mrp_pushnil(L);
         mrp_pushstring(L, "udp sendto:err!"); 
         mrp_pushnumber(L, sent);
         tcp->state = MRSOCK_ERR;
         //MRDBGPRINTF("sendto, error");
      }
   }else{
       mrp_pushnumber(L, 0);
       mrp_pushnil(L);
       mrp_pushnil(L);
   }
   //MRDBGPRINTF("after send,sent = %d", sent);
   return 3;
}



static int meth_receive(mrp_State *L) {
   p_tcp tcp = (p_tcp) toptcp(L, 1);
   //int top = mrp_gettop(L);
   int32 got;
   int32 len = mrp_tonumber(L, 2);

   len = (len > sizeof(tcp->buf))? sizeof(tcp->buf):len;
   if(tcp->sock!=MR_FAILED)
   {
      got = mr_recv(tcp->sock, tcp->buf, len);
   }else
   {
      got = MR_FAILED;
   }
   if((got > 0)&&(got <= len)){
      mrp_pushlstring(L, (const char*)tcp->buf, got);
      mrp_pushnil(L);
      mrp_pushnil(L);
   }else if(got == 0){
      mrp_pushstring(L, "");
      mrp_pushnil(L);
      mrp_pushnil(L);
   }else{
      mrp_pushnil(L);
      mrp_pushstring(L, "tcp recv:err!"); 
      mrp_pushnil(L);
      tcp->state = MRSOCK_ERR;
      //MRDBGPRINTF("recv, error");
   }

   return 3;
}

/*-------------------------------------------------------------------------*\
* Receives data and sender from a UDP socket
\*-------------------------------------------------------------------------*/
static int meth_receivefrom(mrp_State *L) {
   p_tcp tcp = (p_tcp) toptcp(L, 1);
   int32 got;
   int32 ip;
   uint16 port;
   
   int32 len = mrp_tonumber(L, 2);

   len = (len > sizeof(tcp->buf))? sizeof(tcp->buf):len;
   if(tcp->sock!=MR_FAILED)
   {
      got = mr_recvfrom(tcp->sock, tcp->buf, len, &ip, &port);
   }else
   {
      got = MR_FAILED;
   }
   if((got > 0)&&(got <= len)){
      mrp_pushlstring(L, (const char*)tcp->buf, got);
      mrp_pushnumber(L, ip);
      mrp_pushnumber(L, port);
   }else if(got == 0){
      mrp_pushstring(L, "");
      mrp_pushnil(L);
      mrp_pushnil(L);
   }else{
      mrp_pushnil(L);
      mrp_pushstring(L, "udp recvfrom:err!"); 
      mrp_pushnil(L);
      tcp->state = MRSOCK_ERR;
      //MRDBGPRINTF("receivefrom, error");
   }

   return 3;
}



//*************************************
static void setfield (mrp_State *L, const char *key, int value) {
  mrp_pushstring(L, key);
  mrp_pushnumber(L, value);
  mrp_rawset(L, -3);
}

static int meth_getinfo(mrp_State *L)
{
   p_tcp tcp = (p_tcp) toptcp(L, 1);
    mrp_newtable(L);
    setfield(L, "state", tcp->state);
    //setfield(L, "ScreenH", tcp->ScrH);
    return 1;
}



//*************************************
static int meth_getstate(mrp_State *L)
{
   p_tcp tcp = (p_tcp) toptcp(L, 1);
   int32 ret;

   if(tcp->state == MRSOCK_CONNECTING){
      ret = mr_plat(1001, tcp->sock);
      if (ret == MR_SUCCESS){
         tcp->state = MRSOCK_CONNECTED;
      }else if(ret == MR_FAILED){
         tcp->state = MRSOCK_ERR;
         //MRDBGPRINTF("getstate, error");
      }
   }
   mrp_pushnumber(L, tcp->state);
   return 1;
}

static int meth_getsock(mrp_State *L)
{
   p_tcp tcp = (p_tcp) toptcp(L, 1);
    mrp_pushnumber(L, tcp->sock);
    return 1;
}



#if 0
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


static int32 inet_aton(const char *cp)
{
    unsigned int a = 0, b = 0, c = 0, d = 0;
    int n = 0, r;
    unsigned long int addr = 0;
    r = sscanf(cp, "%u.%u.%u.%u%n", &a, &b, &c, &d, &n);
    if (r == 0 || n == 0) return 0;
    cp += n;
    if (*cp) return 0;
    if (a > 255 || b > 255 || c > 255 || d > 255) return 0;
     addr += a; addr <<= 8;
     addr += b; addr <<= 8;
     addr += c; addr <<= 8;
     addr += d;
    return addr;
}

#endif

static int meth_connect(mrp_State *L)
{
    p_tcp tcp = (p_tcp) toptcp(L, 1);
    int32 ip =  mr_L_checknumber(L, 2);
    uint16 port = (uint16) mr_L_checknumber(L, 3);
    int32 type = (int32) mr_L_optint(L, 4, MR_SOCKET_BLOCK);
    int ret;

    ret = mr_connect(tcp->sock, ip, port, type);
    if(MR_FAILED == ret)
   {
       MRDBGPRINTF("mr_connect failed!");
       mrp_pushnil(L);
       tcp->state = MRSOCK_ERR;
       //MRDBGPRINTF("connect, error");
       //mrp_pushnumber(L, ISOCKET_GetLastError(tcp->sock));
       return 1;
   }
   if(MR_WAITING == ret)
   {
      tcp->state = MRSOCK_CONNECTING;
      mrp_pushnumber(L, MR_WAITING);
   }else{
      tcp->state = MRSOCK_CONNECTED;
      mrp_pushnumber(L, MR_SUCCESS);
   }
    
    return 1;
}

/*-------------------------------------------------------------------------*\
* Closes socket used by object 
\*-------------------------------------------------------------------------*/
static int meth_close(mrp_State *L)
{
    p_tcp tcp = (p_tcp) toptcp(L, 1);
    if (tcp->sock!=MR_FAILED)
   {
       mr_closeSocket(tcp->sock);
       //ISOCKET_Release(tcp->sock);
       tcp->sock = MR_FAILED;
   }
    tcp->state = MRSOCK_CLOSED;
    //mrp_pushnumber(L, 1);
    return 0;
}

static int meth_bind(mrp_State *L)
{
    p_tcp tcp = (p_tcp) toptcp(L, 1);
    mr_bind_st add;
    int ret;
    add.socket = tcp->sock;
    add.port =  mr_L_checknumber(L, 2);
    ret = mr_platEx(1003, (uint8*)&add, sizeof(add), (uint8**)NULL, 0, NULL);
    mrp_pushnumber(L, ret);
    return 1;
}


/*=========================================================================*\
* Library functions
\*=========================================================================*/
/*-------------------------------------------------------------------------*\
* Initializes module 
\*-------------------------------------------------------------------------*/
static int global_create(mrp_State *L)
{
   int32 ret = mr_socket(MR_SOCK_STREAM, MR_IPPROTO_TCP);
   MRDBGPRINTF("mr_socket, %d", ret);
   if(MR_FAILED != ret)
   {
      /* allocate tcp object */
      p_tcp tcp = (p_tcp) mrp_newuserdata(L, sizeof(t_tcp));


      mr_L_getmetatable(L, TCPHANDLE);
      mrp_setmetatable(L, -2);

#if 0
      
      /* set its type as master object */
      mr_aux_setclass(L, "tcp{master}", -1);
      /* initialize remaining structure fields */
#endif


      
      tcp->sock = ret;
      tcp->state = MRSOCK_OPENED;
      return 1;
   } else {
       mrp_pushnil(L);
       //mrp_pushnumber(L, INETMGR_GetLastError(pLegendGame->m_piNet) );
       return 1;
   }
}

static int global_udp_create(mrp_State *L)
{
   int32 ret = mr_socket(MR_SOCK_DGRAM, MR_IPPROTO_UDP);
   if(MR_FAILED != ret)
   {
      /* allocate tcp object */
      p_tcp tcp = (p_tcp) mrp_newuserdata(L, sizeof(t_tcp));


      mr_L_getmetatable(L, UDPHANDLE);
      mrp_setmetatable(L, -2);

      tcp->sock = ret;
      tcp->state = MRSOCK_OPENED;
      return 1;
   } else {
       mrp_pushnil(L);
       return 1;
   }
}


