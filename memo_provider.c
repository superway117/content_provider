
#include "content_provider.h"

#include "memo_provider.h"
#include "content_provider_internal.h"
#include "contact_provider_base.h"
typedef struct
{
    DatabaseHandle* db_handle;

}MemoProvider;



#define MEMO_DB_PATH "/nvram/memo.db"

#define MEMO_TABLE_NAME "memo_tb"
static const CHAR *s_memo_schema = "CREATE TABLE memo_tb (ID INTEGER PRIMARY KEY autoincrement, time DATETIME DEFAULT current_timestamp , caption TEXT DEFAULT NULL, content TEXT DEFAULT NULL);";


static INT32 _memo_provider_create_table_impl(DatabaseHandle* db_handle,const char* tb_name,const CHAR* schema_str,BOOL force_new_table)
{


    if (!force_new_table && db_table_is_exist(db_handle,tb_name))
        return DB_OK;

    if (force_new_table)
    {
        db_table_drop(db_handle,tb_name);
    }

    return db_execSQL(db_handle,(const char*)schema_str);
}

static INT32 _memo_provider_create_tables(DatabaseHandle* db_handle)
{

    return _memo_provider_create_table_impl(db_handle,MEMO_TABLE_NAME,s_memo_schema,FALSE);


}


static const CHAR* memo_provider_get_id()
{
    return MEMO_PROVIDER_ID;
}

static INT32 memo_provider_create()
{
    DatabaseHandle* db_handle = NULL;
    INT32 result = DB_OK;
    result=db_open(MEMO_DB_PATH,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    result=_memo_provider_create_tables(db_handle);
    result=db_close(db_handle);
    return result;
}

static INT32 memo_provider_open(ContentProvider* thiz)
{
    DatabaseHandle* db_handle = NULL;
    MemoProvider* sub_provider = NULL;
    INT32 result = DB_OK;

    INT32 flag=DB_OPEN_READWRITE|DB_OPEN_CREATE;
    if(thiz->read_only)
	flag=DB_OPEN_READONLY;
    result=db_open_v2(MEMO_DB_PATH,flag,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    thiz->priv_data = TG_CALLOC_V2(sizeof(MemoProvider));
    sub_provider=(MemoProvider*)thiz->priv_data ;
    sub_provider->db_handle=db_handle;
    return result;
}


static INT32 memo_provider_close(ContentProvider* thiz)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    INT32 result=db_close(sub_provider->db_handle);
    if (result == DB_OK)
        TG_FREE(thiz->priv_data);
    return result;
}

static INT32 memo_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    INT32 db_ret=DB_OK;
    INT32 count=0;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);

    db_ret = db_table_get_count_v2(db_handle,table,where_clause,&count);

    return_val_if_fail(db_ret==DB_OK, 0);
    return count;

}

static INT32 memo_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest)
{

    return memo_provider_get_count_v2(thiz,dest,NULL);

}

#if 0
static INT32 memo_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num, ContentValues* values)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    INT32 db_ret=DB_OK;
    DatabaseStatement* stmt = NULL;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);



    stmt = db_stmt_build_insert_statement(db_handle,table,col_list,col_num);
    if (stmt)
    {
        db_ret= db_stmt_bind_content_values( stmt,values);
        if (db_ret==DB_OK)
            db_ret=db_stmt_step(stmt);
        db_stmt_finalize(stmt);
    }

    return db_ret;

}
#else
static INT32 memo_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num, ContentValues* values)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    INT32 db_ret=DB_OK;
    
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);



    return db_insert(db_handle,table,values);
}
#endif

static Cursor* memo_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    Cursor* cursor =NULL;

    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, NULL);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, NULL);

    cursor = db_query(db_handle ,table,col_list,clause,order_by,limit);

    return cursor;

}

static Cursor* memo_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by, DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    Cursor* cursor =NULL;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, NULL);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, NULL);


    cursor =  db_query_v2(db_handle ,table,col_list,clause,group_by,having,order_by,limit);

    return cursor;

}

static INT32 memo_provider_update(ContentProvider* thiz,ContentProviderDestination dest, ContentValues* values, DatabaseWhereClause* filter)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    INT32 db_ret=DB_OK;

    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);


    db_ret =  db_update(db_handle ,table,values,filter);
    return db_ret;

}

static INT32 memo_provider_delete(ContentProvider* thiz,ContentProviderDestination dest,DatabaseWhereClause* filter)
{

    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    const CHAR* table = MEMO_TABLE_NAME;
    INT32 db_ret=DB_OK;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);

    db_ret = db_delete(db_handle ,table,filter);

    return db_ret;

}
static INT32 memo_provider_last_error(ContentProvider* thiz)
{
    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);
    return db_last_error(db_handle);

}
static DatabaseHandle* memo_provider_get_db_handle(ContentProvider* thiz)
{
    MemoProvider* sub_provider =(MemoProvider*) thiz->priv_data;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, NULL);
    db_handle = sub_provider->db_handle;

    return db_handle;

}
static INT32 memo_provider_release_db_handle(ContentProvider* thiz)
{

    return DB_OK;

}

ContentProviderProcess g_memo_procs = {
    memo_provider_get_id,
    memo_provider_create,
    memo_provider_open,
    memo_provider_close,

    memo_provider_insert,
    memo_provider_query,
    memo_provider_query_v2,
    memo_provider_update,
    memo_provider_delete,

    memo_provider_get_count,
    memo_provider_get_count_v2,

    memo_provider_last_error,

    memo_provider_get_db_handle,
    memo_provider_release_db_handle,
};


