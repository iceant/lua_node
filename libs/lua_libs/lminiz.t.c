#include <lminiz.h>
#include <stdlib.h>
#include <stdio.h>

static const luaL_Reg libs[]={
        {"miniz", luaopen_miniz},
        {NULL, NULL}
};

////////////////////////////////////////////////////////////////////////////////
////
static int lminiz_traceback(lua_State *L) {
    if (!lua_isstring(L, 1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    lua_pushglobaltable(L);
    lua_getfield(L, -1, "debug");
    lua_remove(L, -2);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);  /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);  /* call debug.traceback */
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
////

int main(int argc, char** argv){
    int errfunc ;
    int ret = 0;
    
    lua_State *L = luaL_newstate();
    
    luaL_openlibs(L);
    
    lua_bind_preload(L, libs);
    
    lua_pushcfunction(L, lminiz_traceback);
    errfunc = lua_gettop(L);
    
    if(luaL_loadstring(L, "return require('init')(...)")){
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        ret = -1;
        goto __exit;
    }
    
    lua_createtable(L, argc, 0);
    for(int index = 0; index< argc; index++){
        lua_pushstring(L, argv[index]);
        lua_rawseti(L, -2, index);
    }
    
    
    if(lua_pcall(L, 1, 1, errfunc)){
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        ret = -2;
        goto __exit;
    }

    ret = 0;
    if(lua_type(L, -1)==LUA_TNUMBER){
        ret = (int) lua_tointeger(L, -1);
    }
    
__exit:
    lua_close(L);
    return ret;
}