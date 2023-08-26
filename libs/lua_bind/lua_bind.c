#include <lua_bind.h>
#include <assert.h>

void lua_bind_strings(lua_State* L, lua_bind_string_t values[])
{
    assert(L);
    assert(values);
    ln_size_t i=0;

    while(1){
        lua_bind_string_t value = values[i++];
        if(value.name==NULL){
            break;
        }
        lua_pushstring(L, value.name);
        lua_pushstring(L, value.value);
        lua_settable(L, -3);
    }
}

void lua_bind_integers(lua_State* L, lua_bind_integer_t values[])
{
    assert(L);
    assert(values);
    ln_size_t i=0;
    while(1){
        lua_bind_integer_t value = values[i++];
        if(value.name==NULL){
            break;
        }
        lua_pushstring(L, value.name);
        lua_pushinteger(L, value.value);
        lua_settable(L, -3);
    }
}

void lua_bind_numbers(lua_State* L, lua_bind_number_t values[])
{
    assert(L);
    assert(values);
    ln_size_t i=0;
    while(1){
        lua_bind_number_t value = values[i++];
        if(value.name==NULL){
            break;
        }
        lua_pushstring(L, value.name);
        lua_pushnumber(L, value.value);
        lua_settable(L, -3);
    }
}

void lua_bind_bools(lua_State* L, lua_bind_bool_t values[])
{
    assert(L);
    assert(values);
    ln_size_t i=0;
    while(1){
        lua_bind_bool_t value = values[i++];
        if(value.name==NULL){
            break;
        }
        lua_pushstring(L, value.name);
        lua_pushboolean(L, value.value);
        lua_settable(L, -3);
    }
}

void lua_bind_libs(lua_State* L, const luaL_Reg libs[])
{
    const luaL_Reg *lib;
    /* "require" functions from 'libs' and set results to global table */
    for (lib = libs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }
}

void lua_bind_loaded(lua_State* L, const luaL_Reg libs[])
{
    const luaL_Reg * lib;
    int top = lua_gettop(L);
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaded");
    
    for(lib = libs; lib->func && lib->name; lib++){
        lua_pushcfunction(L, lib->func);
        lua_call(L, 0, 1);
        lua_setfield(L, -2, lib->name);
    }
    lua_settop(L, top);
}

void lua_bind_preload(lua_State* L, const luaL_Reg libs[])
{
    const luaL_Reg * lib;
    int top = lua_gettop(L);
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_remove(L, -2);
    for(lib = libs; lib->func && lib->name; lib++){
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_settop(L, top);
}

////////////////////////////////////////////////////////////////////////////////
////
static void lua_bind_dump_table(lua_State* L, int index, int level);

static void lua_bind_dump_type(lua_State* L, int index, int type, int level)
{
    switch (type) {
        case LUA_TNUMBER:
            printf(" %f", lua_tonumber(L, index));
            break;
        case LUA_TSTRING:
            printf(" %s", lua_tostring(L, index));
            break;
        case LUA_TFUNCTION:
            if (lua_iscfunction(L, index)) {
                printf(" C:%p", lua_tocfunction(L, index));
            }
            break;
        case LUA_TTABLE:
            lua_bind_dump_table(L, index, level+1);
    }
}

static void lua_bind_dump_table(lua_State* L, int index, int level){
    printf("\n{");
    int hasValue = 0;
    lua_pushnil(L); /*first key*/
    while(lua_next(L, index)!=0){
        hasValue = 1;
        int key_type = lua_type(L, -2);
        int val_type = lua_type(L, -1);
        printf("\n");
        for(int i=0; i<level; i++){
            printf(" ");
        }
        switch (key_type) {
            case LUA_TNUMBER:
                printf(" %lld =", lua_tointeger(L, -2));
                break;
            case LUA_TSTRING:
                printf(" ['%s'] =", lua_tostring(L, -2));
                break;
        }

        lua_bind_dump_type(L, -1, val_type, level);

        /* removes 'value' keeps 'key' for next iteration*/
        lua_pop(L, 1);
    }
    if(hasValue){
        printf("\n");
    }
    printf("}\n");
}

void lua_bind_stack_dump(lua_State* L, const char* file, int line)
{
    int top = lua_gettop(L);
    int level = 0;
    printf("==== STACK DUMP ====\n");
    printf("(%s,%d) top=%d\n", file, line, top);
    for (int i = 0; i < top; i++) {
        int positive = top - i;
        int negative = -(i + 1);
        int type = lua_type(L, positive);
        int typeN = lua_type(L, negative);
        assert(type == typeN);
        const char* typeName = lua_typename(L, type);
        printf("%d/%d: type=%s", positive, negative, typeName);
        switch (type) {
            case LUA_TNUMBER:
                printf(" value=%f", lua_tonumber(L, positive));
                break;
            case LUA_TSTRING:
                printf(" value=%s", lua_tostring(L, positive));
                break;
            case LUA_TFUNCTION:
                if (lua_iscfunction(L, positive)) {
                    printf(" C:%p", lua_tocfunction(L, positive));
                }
                break;
            case LUA_TTABLE:
//                lua_bind_dump_table(L, positive, level+1);
                break;
        }
        printf("\n");
    }
}

