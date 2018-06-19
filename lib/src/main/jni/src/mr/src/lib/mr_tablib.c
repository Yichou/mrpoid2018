
#ifndef BREW_MOD
#include <stddef.h>
#endif

#ifdef BREW_MOD
//#include <stddef.h>//ouli brew
#include "AEEStdLib.h"//ouli brew
#endif

//#define ltablib_c

#include "mr.h"

#include "mr_auxlib.h"
#include "mr_lib.h"
#include "mr_store.h"

#include "mr_mem.h"




#define mr_aux_getn(L,n)	(mr_L_checktype(L, n, MRP_TTABLE), mr_L_getn(L, n))


static int mr_B_foreachi (mrp_State *L) {
  int i;
  int n = mr_aux_getn(L, 1);
  mr_L_checktype(L, 2, MRP_TFUNCTION);
  for (i=1; i<=n; i++) {
    mrp_pushvalue(L, 2);  /* function */
    mrp_pushnumber(L, (mrp_Number)i);  /* 1st argument */
    mrp_rawgeti(L, 1, i);  /* 2nd argument */
    mrp_call(L, 2, 1);
    if (!mrp_isnil(L, -1))
      return 1;
    mrp_pop(L, 1);  /* remove nil result */
  }
  return 0;
}


static int mr_B_foreach (mrp_State *L) {
  mr_L_checktype(L, 1, MRP_TTABLE);
  mr_L_checktype(L, 2, MRP_TFUNCTION);
  mrp_pushnil(L);  /* first key */
  for (;;) {
    if (mrp_next(L, 1) == 0)
      return 0;
    mrp_pushvalue(L, 2);  /* function */
    mrp_pushvalue(L, -3);  /* key */
    mrp_pushvalue(L, -3);  /* value */
    mrp_call(L, 2, 1);
    if (!mrp_isnil(L, -1))
      return 1;
    mrp_pop(L, 2);  /* remove value and result */
  }
}


static int mr_B_getn (mrp_State *L) {
  mrp_pushnumber(L, (mrp_Number)mr_aux_getn(L, 1));
  return 1;
}


static int mr_B_setn (mrp_State *L) {
  mr_L_checktype(L, 1, MRP_TTABLE);
  mr_L_setn(L, 1, mr_L_checkint(L, 2));
  return 0;
}


static int mr_B_tinsert (mrp_State *L) {
  int v = mrp_gettop(L);  /* number of arguments */
  int n = mr_aux_getn(L, 1) + 1;
  int pos;  /* where to insert new element */
  if (v == 2)  /* called with only 2 arguments */
    pos = n;  /* insert new element at the end */
  else {
    pos = mr_L_checkint(L, 2);  /* 2nd argument is the position */
    if (pos > n) n = pos;  /* `grow' array if necessary */
    v = 3;  /* function may be called with more than 3 args */
  }
  mr_L_setn(L, 1, n);  /* new size */
  while (--n >= pos) {  /* move up elements */
    mrp_rawgeti(L, 1, n);
    mrp_rawseti(L, 1, n+1);  /* t[n+1] = t[n] */
  }
  mrp_pushvalue(L, v);
  mrp_rawseti(L, 1, pos);  /* t[pos] = v */
  return 0;
}


static int mr_B_tremove (mrp_State *L) {
  int n = mr_aux_getn(L, 1);
  int pos = mr_L_optint(L, 2, n);
  if (n <= 0) return 0;  /* table is `empty' */
  mr_L_setn(L, 1, n-1);  /* t.n = n-1 */
  mrp_rawgeti(L, 1, pos);  /* result = t[pos] */
  for ( ;pos<n; pos++) {
    mrp_rawgeti(L, 1, pos+1);
    mrp_rawseti(L, 1, pos);  /* t[pos] = t[pos+1] */
  }
  mrp_pushnil(L);
  mrp_rawseti(L, 1, n);  /* t[n] = nil */
  return 1;
}


static int str_concat (mrp_State *L) {
  mr_L_Buffer b;
  size_t lsep;
  const char *sep = mr_L_optlstring(L, 2, "", &lsep);
  int i = mr_L_optint(L, 3, 1);
  int n = mr_L_optint(L, 4, 0);
  mr_L_checktype(L, 1, MRP_TTABLE);
  if (n == 0) n = mr_L_getn(L, 1);
  mr_L_buffinit(L, &b);
  for (; i <= n; i++) {
    mrp_rawgeti(L, 1, i);
    mr_L_argcheck(L, mrp_isstring(L, -1), 1, "table contains non-strings");
    mr_L_addvalue(&b);
    if (i != n)
      mr_L_addlstring(&b, sep, lsep);
  }
  mr_L_pushresult(&b);
  return 1;
}



/*
** {======================================================
** Quicksort
** (based on `Algorithms in MODULA-3', Robert Sedgewick;
**  Addison-Wesley, 1993.)
*/


static void set2 (mrp_State *L, int i, int j) {
  mrp_rawseti(L, 1, i);
  mrp_rawseti(L, 1, j);
}

static int sort_comp (mrp_State *L, int a, int b) {
  if (!mrp_isnil(L, 2)) {  /* function? */
    int res;
    mrp_pushvalue(L, 2);
    mrp_pushvalue(L, a-1);  /* -1 to compensate function */
    mrp_pushvalue(L, b-2);  /* -2 to compensate function and `a' */
    mrp_call(L, 2, 1);
    res = mrp_toboolean(L, -1);
    mrp_pop(L, 1);
    return res;
  }
  else  /* a < b? */
    return mrp_lessthan(L, a, b);
}

static void mr_auxsort (mrp_State *L, int l, int u) {
  while (l < u) {  /* for tail recursion */
    int i, j;
    /* sort elements a[l], a[(l+u)/2] and a[u] */
    mrp_rawgeti(L, 1, l);
    mrp_rawgeti(L, 1, u);
    if (sort_comp(L, -1, -2))  /* a[u] < a[l]? */
      set2(L, l, u);  /* swap a[l] - a[u] */
    else
      mrp_pop(L, 2);
    if (u-l == 1) break;  /* only 2 elements */
    i = (l+u)/2;
    mrp_rawgeti(L, 1, i);
    mrp_rawgeti(L, 1, l);
    if (sort_comp(L, -2, -1))  /* a[i]<a[l]? */
      set2(L, i, l);
    else {
      mrp_pop(L, 1);  /* remove a[l] */
      mrp_rawgeti(L, 1, u);
      if (sort_comp(L, -1, -2))  /* a[u]<a[i]? */
        set2(L, i, u);
      else
        mrp_pop(L, 2);
    }
    if (u-l == 2) break;  /* only 3 elements */
    mrp_rawgeti(L, 1, i);  /* Pivot */
    mrp_pushvalue(L, -1);
    mrp_rawgeti(L, 1, u-1);
    set2(L, i, u-1);
    /* a[l] <= P == a[u-1] <= a[u], only need to sort from l+1 to u-2 */
    i = l; j = u-1;
    for (;;) {  /* invariant: a[l..i] <= P <= a[j..u] */
      /* repeat ++i until a[i] >= P */
      while (mrp_rawgeti(L, 1, ++i), sort_comp(L, -1, -2)) {
        if (i>u) mr_L_error(L, "invalid order function for sorting");
        mrp_pop(L, 1);  /* remove a[i] */
      }
      /* repeat --j until a[j] <= P */
      while (mrp_rawgeti(L, 1, --j), sort_comp(L, -3, -1)) {
        if (j<l) mr_L_error(L, "invalid order function for sorting");
        mrp_pop(L, 1);  /* remove a[j] */
      }
      if (j<i) {
        mrp_pop(L, 3);  /* pop pivot, a[i], a[j] */
        break;
      }
      set2(L, i, j);
    }
    mrp_rawgeti(L, 1, u-1);
    mrp_rawgeti(L, 1, i);
    set2(L, u-1, i);  /* swap pivot (a[u-1]) with a[i] */
    /* a[l..i-1] <= a[i] == P <= a[i+1..u] */
    /* adjust so that smaller half is in [j..i] and larger one in [l..u] */
    if (i-l < u-i) {
      j=l; i=i-1; l=i+2;
    }
    else {
      j=i+1; i=u; u=j-2;
    }
    mr_auxsort(L, j, i);  /* call recursively the smaller one */
  }  /* repeat the routine for the larger one */
}

static int mr_B_sort (mrp_State *L) {
  int n = mr_aux_getn(L, 1);
  mr_L_checkstack(L, 40, "");  /* assume array is smaller than 2^40 */
  if (!mrp_isnoneornil(L, 2))  /* is there a 2nd argument? */
    mr_L_checktype(L, 2, MRP_TFUNCTION);
  mrp_settop(L, 2);  /* make sure there is two arguments */
  mr_auxsort(L, 1, n);
  return 0;
}

/* }====================================================== */

static int mr_B_save (mrp_State *L) {
  WriterInfo wi;
  
  wi.buf = NULL;
  wi.buflen = 0;
  
  mrp_settop(L, 2);
              /*perms? rootTable?  */
  mr_L_checktype(L, 1, MRP_TTABLE);
              /*perms? rootTable */
  mr_L_checktype(L, 2, MRP_TTABLE);
              /*perms rootTable */

  //mrp_insert(L, 1);
              /* perms rootTable */
  
  mr_store_persist(L, mr_str_bufwriter, &wi);
  
  mrp_settop(L, 0);
              /* (empty) */
  mrp_pushlstring(L, wi.buf, wi.buflen);
              /* str */
  mr_M_freearray(L, wi.buf, wi.buflen, char);
  return 1;
}

static int mr_B_load (mrp_State *L) {
   LoadInfo li;
   				/* perms? str? ...? */
   mrp_settop(L, 2);
   				/* perms? str? */
   li.buf = mr_L_checklstring(L, 2, &li.size);
   				/* perms? str */
   mrp_pop(L, 1);
   /* It is conceivable that the buffer might now be collectable,
    * which would cause problems in the reader. I can't think of
    * any situation where there would be no other reference to the
    * buffer, so for now I'll leave it alone, but this is a potential
    * bug. */
   				/* perms? */
   mr_L_checktype(L, 1, MRP_TTABLE);
   				/* perms */
   mr_store_unpersist(L, mr_str_bufreader, &li);
   				/* perms rootobj */
   return 1;
}


static const mr_L_reg tab_funcs[] = {
   {"concat", str_concat},
   {"getArrSize", mr_B_getn},
   {"setArrSize", mr_B_setn},
   {"sort", mr_B_sort},
   {"insert", mr_B_tinsert},
   {"remove", mr_B_tremove},
   {"forEach", mr_B_foreach},
   {"iforEach", mr_B_foreachi},
   {"save", mr_B_save},
   {"load", mr_B_load},
   {"next", mr_B_next},
   {"items", mr_B_unpack},
   {"rawGet", mr_B_rawget},
   {"rawSet", mr_B_rawset},
   {"iPairs", mr_B_ipairs},
   {"pairs", mr_B_pairs},
#ifdef COMPATIBILITY01
   {"foreach", mr_B_foreach},
   {"foreachi", mr_B_foreachi},
   {"getn", mr_B_getn},
   {"setn", mr_B_setn},
#endif
   {NULL, NULL}
};


MRPLIB_API int mrp_open_table (mrp_State *L) {
  mr_L_openlib(L, MRP_TABLIBNAME, tab_funcs, 0);
  return 1;
}

