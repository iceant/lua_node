#ifndef INCLUDED_LMINIZ_H
#define INCLUDED_LMINIZ_H

////////////////////////////////////////////////////////////////////////////////
//// 
#ifndef INCLUDED_LUA_BIND_H
#include <lua_bind.h>
#endif /*INCLUDED_LUA_BIND_H*/

#ifndef INCLUDED_MINIZ_H
#define INCLUDED_MINIZ_H
#include <miniz.h>
#endif /*INCLUDED_MINIZ_H*/

////////////////////////////////////////////////////////////////////////////////
////
#define LMINIZ_STREAM_MODE_DEFLATE 0
#define LMINIZ_STREAM_NODE_INFLATE 1

typedef struct lminiz_file_s{
    mz_zip_archive archive;
}lminiz_file_t;

typedef struct lminiz_stream_s{
    mz_stream stream;
    int mode; /*0=deflate, 1=inflate*/
}lminiz_stream_t;
////////////////////////////////////////////////////////////////////////////////
////
LUALIB_API int luaopen_miniz(lua_State* L);

#endif /* INCLUDED_LMINIZ_H */
