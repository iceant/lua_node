#include <lminiz.h>
#include <stdio.h>
#include <miniz.c>

////////////////////////////////////////////////////////////////////////////////
////
#define LMINIZ_READER_META_NAME "miniz_reader"
#define LMINIZ_WRITER_META_NAME "miniz_writer"
#define LMINIZ_DEFLATOR_META_NAME "miniz_deflator"
#define LMINIZ_INFLATOR_META_NAME "miniz_inflator"

////////////////////////////////////////////////////////////////////////////////
////
mz_bool mz_zip_reader_init_file_v3(mz_zip_archive *pZip, const char *pFilename, mz_uint flags, mz_uint64 file_start_ofs, mz_uint64 archive_size, mz_file_read_func read_func)
{
    mz_uint64 file_size;
    MZ_FILE *pFile;
    
    if ((!pZip) || (!pFilename) || ((archive_size) && (archive_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)))
        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);
    
    pFile = MZ_FOPEN(pFilename, "rb");
    if (!pFile)
        return mz_zip_set_error(pZip, MZ_ZIP_FILE_OPEN_FAILED);
    
    file_size = archive_size;
    if (!file_size)
    {
        if (MZ_FSEEK64(pFile, 0, SEEK_END))
        {
            MZ_FCLOSE(pFile);
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_SEEK_FAILED);
        }
        
        file_size = MZ_FTELL64(pFile);
    }
    
    /* TODO: Better sanity check archive_size and the # of actual remaining bytes */
    
    if (file_size < MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
    {
        MZ_FCLOSE(pFile);
        return mz_zip_set_error(pZip, MZ_ZIP_NOT_AN_ARCHIVE);
    }
    
    if (!mz_zip_reader_init_internal(pZip, flags))
    {
        MZ_FCLOSE(pFile);
        return MZ_FALSE;
    }
    
    pZip->m_zip_type = MZ_ZIP_TYPE_FILE;
    pZip->m_pRead = read_func;
    pZip->m_pIO_opaque = pZip;
    pZip->m_pState->m_pFile = pFile;
    pZip->m_archive_size = file_size;
    pZip->m_pState->m_file_archive_start_ofs = file_start_ofs;
    
    if (!mz_zip_reader_read_central_dir(pZip, flags))
    {
        mz_zip_reader_end_internal(pZip, MZ_FALSE);
        return MZ_FALSE;
    }
    
    return MZ_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//// READER

#if 0
static size_t mz_zip_file_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n)
{
    mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
    mz_int64 cur_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);

    file_ofs += pZip->m_pState->m_file_archive_start_ofs;

    if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        return 0;

    return MZ_FREAD(pBuf, 1, n, pZip->m_pState->m_pFile);
}
#endif
static size_t lminiz_reader_read(void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n)
{
    lminiz_file_t * zip = pOpaque;
    mz_zip_archive* pZip = &zip->archive;
    mz_int64 cur_ofs = MZ_FTELL64(pZip->m_pState->m_pFile);
    file_ofs += pZip->m_pState->m_file_archive_start_ofs;
    if (((mz_int64)file_ofs < 0) || (((cur_ofs != (mz_int64)file_ofs)) && (MZ_FSEEK64(pZip->m_pState->m_pFile, (mz_int64)file_ofs, SEEK_SET))))
        return 0;
    
    return MZ_FREAD(pBuf, 1, n, pZip->m_pState->m_pFile);
}

static int lminiz_reader_init(lua_State* L)
{
    const char* file_path = luaL_checkstring(L , 1);
    mz_uint32 flags = luaL_optinteger(L, 2, 0);
    mz_uint64 size=0;
    lminiz_file_t * zip = lua_newuserdata(L, sizeof(*zip));
    mz_zip_archive * archive = &(zip->archive);
    luaL_getmetatable(L, LMINIZ_READER_META_NAME);
    lua_setmetatable(L, -2);
    
    memset(archive, 0, sizeof(*archive));
    archive->m_pRead = lminiz_reader_read;
    archive->m_pIO_opaque = zip;
    if(!mz_zip_reader_init_file_v3(archive, file_path, flags, 0, size, lminiz_reader_read)){
        lua_pushnil(L);
        lua_pushfstring(L, "read %s failed because of %s", file_path,
                       mz_zip_get_error_string(mz_zip_get_last_error(archive)));
        return 2;
    }
    return 1;
}

static int lminiz_reader_gc(lua_State* L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    mz_zip_reader_end(&zip->archive);
    return 0;
}

static int lminiz_reader_get_num_files(lua_State* L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    lua_pushinteger(L, mz_zip_reader_get_num_files(&zip->archive));
    return 1;
}

static int lminiz_reader_locate_file(lua_State* L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    const char* path = luaL_checkstring(L, 2);
    mz_uint32 flags = luaL_optinteger(L, 3, 0);
    int index = mz_zip_reader_locate_file(&zip->archive, path, NULL, flags);
    if(index<0){
        lua_pushnil(L);
        lua_pushfstring(L, "Can't find file %s.", path);
        return 2;
    }
    lua_pushinteger(L, index+1);
    return 1;
}

static int lminiz_reader_stat(lua_State* L)
{
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    mz_uint  file_index = (mz_uint) luaL_checkinteger(L, 2)-1;
    mz_zip_archive_file_stat stat;
    if(!mz_zip_reader_file_stat(&zip->archive, file_index, &stat)){
        lua_pushnil(L);
        lua_pushfstring(L, "%d is an invalid index", file_index);
        return 2;
    }
    lua_newtable(L);
    lua_pushinteger(L, file_index);
    lua_setfield(L, -2, "index");
    lua_pushinteger(L, stat.m_version_made_by);
    lua_setfield(L, -2, "version_made_by");
    lua_pushinteger(L, stat.m_version_needed);
    lua_setfield(L, -2, "version_needed");
    lua_pushinteger(L, stat.m_bit_flag);
    lua_setfield(L, -2, "bit_flag");
    lua_pushinteger(L, stat.m_method);
    lua_setfield(L, -2, "method");
    lua_pushinteger(L, stat.m_time);
    lua_setfield(L, -2, "time");
    lua_pushinteger(L, stat.m_crc32);
    lua_setfield(L, -2, "crc32");
    lua_pushinteger(L, stat.m_comp_size);
    lua_setfield(L, -2, "comp_size");
    lua_pushinteger(L, stat.m_uncomp_size);
    lua_setfield(L, -2, "uncomp_size");
    lua_pushinteger(L, stat.m_internal_attr);
    lua_setfield(L, -2, "internal_attr");
    lua_pushinteger(L, stat.m_external_attr);
    lua_setfield(L, -2, "external_attr");
    lua_pushstring(L, stat.m_filename);
    lua_setfield(L, -2, "filename");
    lua_pushstring(L, stat.m_comment);
    lua_setfield(L, -2, "comment");
    return 1;
}

static int lminiz_reader_get_filename(lua_State* L)
{
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    mz_uint file_index = (mz_uint) luaL_checkinteger(L, 2)-1;
    char filename[PATH_MAX];
    mz_uint filename_buf_sz = PATH_MAX;
    if(!mz_zip_reader_get_filename(&zip->archive, file_index, filename, filename_buf_sz)){
        lua_pushnil(L);
        lua_pushfstring(L, "%d is an invalid index", file_index);
        return 2;
    }
    lua_pushstring(L, filename);
    return 1;
}

static int lminiz_reader_is_file_a_directory(lua_State* L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    mz_uint file_index = (mz_uint) luaL_checkinteger(L, 2)-1;
    lua_pushboolean(L, mz_zip_reader_is_file_a_directory(&zip->archive, file_index));
    return 1;
}

static int lminiz_reader_extract(lua_State * L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    mz_uint file_index = (mz_uint) luaL_checkinteger(L, 2)-1;
    mz_uint flags = luaL_optinteger(L, 3, 0);
    size_t out_sz;
    char* out_buf = mz_zip_reader_extract_to_heap(&zip->archive, file_index, &out_sz, flags);
    lua_pushlstring(L, out_buf, out_sz);
    MZ_FREE(out_buf);
    return 1;
}

static int lminiz_reader_get_offset(lua_State* L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_READER_META_NAME);
    mz_zip_archive * archive = &zip->archive;
    lua_pushinteger(L, archive->m_pState->m_file_archive_start_ofs);
    return 1;
}
////////////////////////////////////////////////////////////////////////////////
//// WRITER
static int lminiz_writer_init(lua_State * L){
    size_t size_to_reserve_at_beginning = luaL_optinteger(L, 1, 0);
    size_t initial_allocation_size = luaL_optinteger(L, 2, 128 * 1024);
    lminiz_file_t* zip = lua_newuserdata(L, sizeof(*zip));
    mz_zip_archive* archive = &(zip->archive);
    luaL_getmetatable(L, LMINIZ_WRITER_META_NAME);
    lua_setmetatable(L, -2);
    memset(archive, 0, sizeof(*archive));
    if (!mz_zip_writer_init_heap(archive, size_to_reserve_at_beginning, initial_allocation_size)) {
        return luaL_error(L, "Problem initializing heap writer");
    }
    return 1;
}

static int lminiz_writer_gc(lua_State* L){
    lminiz_file_t * zip = luaL_checkudata(L, 1, LMINIZ_WRITER_META_NAME);
    mz_zip_writer_end(&zip->archive);
    return 0;
}


static int lminiz_writer_add_from_zip_reader(lua_State *L) {
    lminiz_file_t* zip = luaL_checkudata(L, 1, LMINIZ_WRITER_META_NAME);
    lminiz_file_t* source = luaL_checkudata(L, 2, LMINIZ_READER_META_NAME);
    mz_uint file_index = (mz_uint)luaL_checkinteger(L, 3) - 1;
    if (!mz_zip_writer_add_from_zip_reader(&(zip->archive), &(source->archive), file_index)) {
        return luaL_error(L, "Failure to copy file between zips");
    }
    return 0;
}

static int lminiz_writer_add_mem(lua_State *L) {
    lminiz_file_t* zip = luaL_checkudata(L, 1, LMINIZ_WRITER_META_NAME);
    const char* path = luaL_checkstring(L, 2);
    size_t size;
    const char* data = luaL_checklstring(L, 3, &size);
    mz_uint flags = luaL_optinteger(L, 4, 0);
    if (!mz_zip_writer_add_mem(&(zip->archive), path, data, size, flags)) {
        return luaL_error(L, "Failure to add entry to zip");
    }
    return 0;
}

static int lminiz_writer_finalize(lua_State *L) {
    lminiz_file_t* zip = luaL_checkudata(L, 1, LMINIZ_WRITER_META_NAME);
    void* data;
    size_t size;
    if (!mz_zip_writer_finalize_heap_archive(&(zip->archive), &data, &size)) {
        luaL_error(L, "Problem finalizing archive");
    }
    lua_pushlstring(L, data, size);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
////

static const char* flush_types[] = {
        "no", "partial", "sync", "full", "finish", "block",
        NULL
};

static int lminiz_inflator_deflator_impl(lua_State* L, lminiz_stream_t* stream) {
    mz_streamp miniz_stream = &(stream->stream);
    size_t data_size;
    const char* data = luaL_checklstring(L, 2, &data_size);
    int flush = luaL_checkoption(L, 3, "no", flush_types);
    miniz_stream->avail_in = data_size;
    miniz_stream->next_in = (const unsigned char*)data;
    luaL_Buffer buf;
    luaL_buffinit(L, &buf);
    do {
        miniz_stream->avail_out = LUAL_BUFFERSIZE;
        miniz_stream->next_out = (unsigned char*)luaL_prepbuffer(&buf);
        int status;
        size_t before = miniz_stream->total_out;
        if (stream->mode) {
            status = mz_inflate(miniz_stream, flush);
        } else {
            status = mz_deflate(miniz_stream, flush);
        }
        size_t added = miniz_stream->total_out - before;
        switch (status) {
            case MZ_OK:
            case MZ_STREAM_END:
                luaL_addsize(&buf, added);
                break;
            case MZ_BUF_ERROR:
                break;
            default:
                lua_pushnil(L);
                lua_pushstring(L, mz_error(status));
                luaL_pushresult(&buf);
                return 3;
        }
    } while (miniz_stream->avail_out == 0);
    luaL_pushresult(&buf);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
//// deflator

static int lminiz_check_compression_level(lua_State* L, int index) {
    int level = luaL_optinteger(L, index, MZ_DEFAULT_COMPRESSION);
    if (level < MZ_DEFAULT_COMPRESSION || level > MZ_BEST_COMPRESSION) {
        luaL_error(L, "Compression level must be between %d and %d", MZ_DEFAULT_COMPRESSION, MZ_BEST_COMPRESSION);
    }
    return level;
}

static int lminiz_deflator_init(lua_State* L) {
    int level = lminiz_check_compression_level(L, 1);
    lminiz_stream_t* stream = lua_newuserdata(L, sizeof(*stream));
    mz_streamp miniz_stream = &(stream->stream);
    luaL_getmetatable(L, LMINIZ_DEFLATOR_META_NAME);
    lua_setmetatable(L, -2);
    memset(miniz_stream, 0, sizeof(*miniz_stream));
    int status = mz_deflateInit(miniz_stream, level);
    if (status != MZ_OK) {
        const char* msg = mz_error(status);
        if (msg) {
            luaL_error(L, "Problem initializing stream: %s", msg);
        } else {
            luaL_error(L, "Problem initializing stream");
        }
    }
    stream->mode = LMINIZ_STREAM_MODE_DEFLATE;
    return 1;
}

static int lminiz_deflator_gc(lua_State* L) {
    lminiz_stream_t* stream = luaL_checkudata(L, 1, LMINIZ_DEFLATOR_META_NAME);
    mz_deflateEnd(&(stream->stream));
    return 0;
}

static int lminiz_deflator_deflate(lua_State* L) {
    lminiz_stream_t* stream = luaL_checkudata(L, 1, LMINIZ_DEFLATOR_META_NAME);
    return lminiz_inflator_deflator_impl(L, stream);
}


////////////////////////////////////////////////////////////////////////////////
//// inflator
static int lminiz_inflator_init(lua_State* L) {
    lminiz_stream_t* stream = lua_newuserdata(L, sizeof(*stream));
    mz_streamp miniz_stream = &(stream->stream);
    luaL_getmetatable(L, LMINIZ_INFLATOR_META_NAME);
    lua_setmetatable(L, -2);
    memset(miniz_stream, 0, sizeof(*miniz_stream));
    int status = mz_inflateInit(miniz_stream);
    if (status != MZ_OK) {
        const char* msg = mz_error(status);
        if (msg) {
            luaL_error(L, "Problem initializing stream: %s", msg);
        } else {
            luaL_error(L, "Problem initializing stream");
        }
    }
    stream->mode = LMINIZ_STREAM_NODE_INFLATE;
    return 1;
}

static int lminiz_inflator_gc(lua_State* L) {
    lminiz_stream_t* stream = luaL_checkudata(L, 1, LMINIZ_INFLATOR_META_NAME);
    mz_inflateEnd(&(stream->stream));
    return 0;
}

static int lminiz_inflator_inflate(lua_State* L) {
    lminiz_stream_t* stream = luaL_checkudata(L, 1, LMINIZ_INFLATOR_META_NAME);
    return lminiz_inflator_deflator_impl(L, stream);
}

////////////////////////////////////////////////////////////////////////////////
//// miniz methods

static int ltinfl(lua_State* L) {
    size_t in_len;
    const char* in_buf = luaL_checklstring(L, 1, &in_len);
    size_t out_len;
    int flags = luaL_optinteger(L, 2, 0);
    char* out_buf = tinfl_decompress_mem_to_heap(in_buf, in_len, &out_len, flags);
    lua_pushlstring(L, out_buf, out_len);
    free(out_buf);
    return 1;
}

static int ltdefl(lua_State* L) {
    size_t in_len;
    const char* in_buf = luaL_checklstring(L, 1, &in_len);
    size_t out_len;
    int flags = luaL_optinteger(L, 2, 0);
    char* out_buf = tdefl_compress_mem_to_heap(in_buf, in_len, &out_len, flags);
    lua_pushlstring(L, out_buf, out_len);
    free(out_buf);
    return 1;
}

static int lminiz_adler32(lua_State* L) {
    mz_ulong adler = luaL_optinteger(L, 1, 0);
    size_t buf_len = 0;
    const unsigned char* ptr = (const unsigned char*)luaL_optlstring(L, 2, NULL, &buf_len);
    adler = mz_adler32(adler, ptr, buf_len);
    lua_pushinteger(L, adler);
    return 1;
}

static int lminiz_crc32(lua_State* L) {
    mz_ulong crc32_val = luaL_optinteger(L, 1, 0);
    size_t buf_len = 0;
    const unsigned char* ptr = (const unsigned char*)luaL_optlstring(L, 2, NULL, &buf_len);
    crc32_val = mz_crc32(crc32_val, ptr, buf_len);
    lua_pushinteger(L, crc32_val);
    return 1;
}

static int lminiz_version(lua_State* L) {
    lua_pushstring(L, mz_version());
    return 1;
}

static int lminiz_compress(lua_State* L)
{
    int level, ret;
    size_t in_len, out_len;
    const unsigned char *inb;
    unsigned char *outb;
    in_len = 0;
    inb = (const unsigned char *)luaL_checklstring(L, 1, &in_len);
    level = lminiz_check_compression_level(L, 2);
    out_len = mz_compressBound(in_len);
    outb = malloc(out_len);
    ret = mz_compress2(outb, (mz_ulong*)&out_len, inb, in_len, level);
    switch (ret) {
        case MZ_OK:
            lua_pushlstring(L, (const char*)outb, out_len);
            ret = 1;
            break;
        default:
            lua_pushnil(L);
            lua_pushstring(L, mz_error(ret));
            ret = 2;
            break;
    }
    free(outb);
    return ret;
}

static int lminiz_uncompress(lua_State* L)
{
    int ret;
    size_t in_len, out_len;
    const unsigned char* inb;
    unsigned char* outb;
    in_len = 0;
    inb = (const unsigned char*)luaL_checklstring(L, 1, &in_len);
    out_len = luaL_optinteger(L, 2, (lua_Integer)(in_len * 2));
    if (out_len < 1 || out_len > INT_MAX) {
        luaL_error(L, "Initial buffer size must be between 1 and %d", INT_MAX);
    }
    do {
        outb = malloc(out_len);
        ret = mz_uncompress(outb, (mz_ulong*)&out_len, inb, in_len);
        if (ret == MZ_BUF_ERROR) {
            out_len *= 2;
            free(outb);
        } else {
            break;
        }
    } while (out_len > 1 && out_len < INT_MAX);
    switch (ret) {
        case MZ_OK:
            lua_pushlstring(L, (const char*)outb, out_len);
            ret = 1;
            break;
        default:
            lua_pushnil(L);
            lua_pushstring(L, mz_error(ret));
            ret = 2;
            break;
    }
    free(outb);
    return ret;
}



////////////////////////////////////////////////////////////////////////////////
////
static const luaL_Reg lminiz_read_m[] = {
        {"get_num_files", lminiz_reader_get_num_files},
        {"stat", lminiz_reader_stat},
        {"get_filename", lminiz_reader_get_filename},
        {"is_directory", lminiz_reader_is_file_a_directory},
        {"extract", lminiz_reader_extract},
        {"locate_file", lminiz_reader_locate_file},
        {"get_offset", lminiz_reader_get_offset},
        {NULL, NULL}
};

static const luaL_Reg lminiz_write_m[] = {
        {"add_from_zip", lminiz_writer_add_from_zip_reader},
        {"add", lminiz_writer_add_mem},
        {"finalize", lminiz_writer_finalize},
        {NULL, NULL}
};

static const luaL_Reg lminiz_deflate_m[] = {
        {"deflate", lminiz_deflator_deflate},
        {NULL,NULL}
};

static const luaL_Reg lminiz_inflate_m[] = {
        {"inflate", lminiz_inflator_inflate},
        {NULL,NULL}
};

static const luaL_Reg lminiz_f[] = {
        {"new_reader", lminiz_reader_init},
        {"new_writer", lminiz_writer_init},
        {"inflate", ltinfl},
        {"deflate", ltdefl},
        {"adler32", lminiz_adler32},
        {"crc32", lminiz_crc32},
        {"compress", lminiz_compress},
        {"uncompress", lminiz_uncompress},
        {"version", lminiz_version},
        {"new_deflator", lminiz_deflator_init},
        {"new_inflator", lminiz_inflator_init},
        {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
//// INTERFACE

LUALIB_API int luaopen_miniz(lua_State* L)
{
    luaL_newmetatable(L, LMINIZ_READER_META_NAME);
    luaL_newlib(L, lminiz_read_m);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, lminiz_reader_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);
    luaL_newmetatable(L, LMINIZ_WRITER_META_NAME);
    luaL_newlib(L, lminiz_write_m);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, lminiz_writer_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);
    luaL_newmetatable(L, LMINIZ_DEFLATOR_META_NAME);
    luaL_newlib(L, lminiz_deflate_m);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, lminiz_deflator_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);
    luaL_newmetatable(L, LMINIZ_INFLATOR_META_NAME);
    luaL_newlib(L, lminiz_inflate_m);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, lminiz_inflator_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);
    luaL_newlib(L, lminiz_f);
    return 1;
}