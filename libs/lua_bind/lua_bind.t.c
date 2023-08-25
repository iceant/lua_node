#include <lua_bind.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

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

////////////////////////////////////////////////////////////////////////////////
//// DIR
static int dir_iter(lua_State *L);

static int dir_new(lua_State *L){
    const char * path = luaL_checkstring(L, 1);
    DIR** d = (DIR**) lua_newuserdata(L, sizeof(*d));
    *d = NULL;
    luaL_getmetatable(L, "dir.meta");
    lua_setmetatable(L, -2);

    *d = opendir(path);
    if(*d==NULL){
        luaL_error(L, "can not open %s: %s", path, strerror(errno));
    }

    lua_pushcclosure(L, dir_iter, 1);

    return 1;
}

static int is_regular_file(const char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

static int dir_iter(lua_State *L){
    DIR* d = *(DIR**) lua_touserdata(L, lua_upvalueindex(1));
    struct dirent* entry = readdir(d);
    if(entry!=NULL){
        lua_pushstring(L, entry->d_name);
        return 1;
    }else{
        return 0;
    }
}

static int dir_is_file(lua_State *L){
    const char* path = luaL_checkstring(L, 1);
    if(path){
        lua_pushboolean(L, is_regular_file(path));
        return 1;
    }else{
        return 0;
    }
}

static int dir_gc(lua_State *L){
    DIR* d = *(DIR**) lua_touserdata(L, 1);
    if(d){
        closedir(d);
    }
    return 0;
}

static const struct luaL_Reg dir_object_methods[]={
        {"open", dir_new},
        {"is_file", dir_is_file},
        {NULL, NULL}
};

static int luaopen_dir(lua_State *L){
    luaL_newmetatable(L, "dir.meta");
    lua_pushcfunction(L, dir_gc);
    lua_setfield(L, -2, "__gc");

    luaL_newlib(L, dir_object_methods);
    return 1;
}
////////////////////////////////////////////////////////////////////////////////
////



static luaL_Reg libs[]={
        {"test", luaopen_testlib},
        {"dir", luaopen_dir},
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