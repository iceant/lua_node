#ifndef INCLUDED_LUA_BIND_H
#define INCLUDED_LUA_BIND_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_LN_TYPES_H
#include <ln_types.h>
#endif /*INCLUDED_LN_TYPES_H*/

#ifndef INCLUDED_LUA_H
#define INCLUDED_LUA_H
#include <lua.h>
#endif /*INCLUDED_LUA_H*/

#ifndef INCLUDED_LUALIB_H
#define INCLUDED_LUALIB_H
#include <lualib.h>
#endif /*INCLUDED_LUALIB_H*/

#ifndef INCLUDED_LAUXLIB_H
#define INCLUDED_LAUXLIB_H
#include <lauxlib.h>
#endif /*INCLUDED_LAUXLIB_H*/

////////////////////////////////////////////////////////////////////////////////
////
typedef struct lua_bind_string_s{
    char* name;
    char* value;
}lua_bind_string_t;

typedef struct lua_bind_integer_s{
    char* name;
    lua_Integer value;
}lua_bind_integer_t;

typedef struct lua_bind_number_s{
    char* name;
    lua_Number value;
}lua_bind_number_t;

typedef struct lua_bind_bool_s{
    char* name;
    int value;
}lua_bind_bool_t;

////////////////////////////////////////////////////////////////////////////////
////
void lua_bind_strings(lua_State* L, lua_bind_string_t values[]);
void lua_bind_integers(lua_State* L, lua_bind_integer_t values[]);
void lua_bind_numbers(lua_State* L, lua_bind_number_t values[]);
void lua_bind_bools(lua_State* L, lua_bind_bool_t values[]);

void lua_bind_libs(lua_State* L, luaL_Reg libs[]);

void lua_bind_stack_dump(lua_State* L, const char* file, int line);

////////////////////////////////////////////////////////////////////////////////
////
#define LUA_BIND_DUMP(L) lua_bind_stack_dump(L, __FILE__, __LINE__)


#endif /*INCLUDED_LUA_BIND_H*/
