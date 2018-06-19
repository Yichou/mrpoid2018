

#include <string.h>

//#define ltm_c

#include "mr.h"

#include "mr_object.h"
#include "mr_state.h"
#include "mr_string.h"
#include "mr_table.h"
#include "mr_tm.h"



const char *const mr_T_typenames[] = {
  "nil", "boolean", "object", "number",
  "string", "table", "function", "object", "thread"
};

const char *const mr_T_short_typenames[] = {
  "nil", "bool", "obj", "num",
  "str", "tab", "func", "obj", "co"
};



void mr_T_init (mrp_State *L) {
#if 0
  static const char *const mr_T_eventname[] = {  /* ORDER TM */ 
    "__index", "__newindex",
    "__gc", "__mode", "__eq",
    "__add", "__sub", "__mul", "__div",
    "__pow", "__unm", "__lt", "__le",
    "__concat", "__call"
  };
#endif //ouli brew
  static const char *const mr_T_eventname[] = {  /* ORDER TM */
    "__index", "__newindex",
    "__gc", "__mode", "__eq",
    "__add", "__sub", "__mul", "__div",
    "__op", "__unm", "__lt", "__le",
    "__concat", "__call"
  };
  int i;
  for (i=0; i<TM_N; i++) {
    G(L)->tmname[i] = mr_S_new(L, mr_T_eventname[i]);
    mr_S_fix(G(L)->tmname[i]);  /* never collect these names */
  }
}


/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
const TObject *mr_T_gettm (Table *events, TMS event, TString *ename) {
  const TObject *tm = mr_H_getstr(events, ename);
  mrp_assert(event <= TM_EQ);
  if (ttisnil(tm)) {  /* no tag method? */
    events->flags |= cast(lu_byte, 1u<<event);  /* cache this fact */
    return NULL;
  }
  else return tm;
}


const TObject *mr_T_gettmbyobj (mrp_State *L, const TObject *o, TMS event) {
  TString *ename = G(L)->tmname[event];
  switch (ttype(o)) {
    case MRP_TTABLE:
      return mr_H_getstr(hvalue(o)->metatable, ename);
    case MRP_TUSERDATA:
      return mr_H_getstr(uvalue(o)->uv.metatable, ename);
    default:
      return &mr_O_nilobject;
  }
}

