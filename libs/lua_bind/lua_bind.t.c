#include <lua_bind.h>
#include <stdlib.h>
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
////
static int lua_bind_test_add(lua_State *L){
    LUA_BIND_DUMP(L);
    if(lua_gettop(L)!=2){
        printf("parameter number is not 2!\n");
        return 0;
    }
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a+b);
    return 1;
}

static lua_bind_string_t strings[]={
        {"OS_NAME", "Windows"},
        {NULL, 0}
};

static lua_bind_integer_t integers[]={
        {"UINT32_MAX", UINT32_MAX},
        {NULL, 0}
};

static luaL_Reg methods[]={
        {"add", lua_bind_test_add},
        {NULL, 0}
};



static int luaopen_testlib(lua_State *L){
    lua_newtable(L);
    LUA_BIND_DUMP(L);
    lua_bind_strings(L, strings);
    lua_bind_integers(L, integers);
    luaL_setfuncs(L, methods, 0);
    LUA_BIND_DUMP(L);
    return 1;
}

static luaL_Reg libs[]={
        {"test", luaopen_testlib},
        {NULL, NULL}
};
////////////////////////////////////////////////////////////////////////////////
////



int main(int argc, char** argv)
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    lua_bind_libs(L, libs);
    LUA_BIND_DUMP(L);

    if(luaL_dostring(L, "return require('init')(...)")){
        printf("failed!\n");
    }

    lua_close(L);

    return 0;
}