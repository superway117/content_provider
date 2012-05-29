
#include "content_provider.h"

#include "sms_provider.h"
#include "content_provider_internal.h"

typedef struct
{
    DatabaseHandle* db_handle;
}SmsProvider;




static INT32 _sms_provider_create_tables(DatabaseHandle* db_handle)
{

    INT32 result = DB_OK;
    //sms table
    result=sms_create_tables(db_handle);
    result=sms_create_common_tables(db_handle);
    result=sms_create_triggers(db_handle);
    result=sms_create_index(db_handle);
    return result;
}


static const CHAR* sms_provider_get_id()
{
    return SMS_PROVIDER_ID;
}

static INT32 sms_provider_create()
{
    DatabaseHandle* db_handle = NULL;
    INT32 result = DB_OK;

    result=db_open(SMS_DB_PATH,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    result=_sms_provider_create_tables(db_handle);
    result=db_close(db_handle);
    return result;
}

static INT32 sms_provider_open(ContentProvider* thiz)
{
    DatabaseHandle* db_handle = NULL;
    SmsProvider* sub_provider = NULL;
    INT32 result = DB_OK;

    INT32 flag=DB_OPEN_READWRITE|DB_OPEN_CREATE;
    if (thiz->read_only)
        flag = DB_OPEN_READONLY;
    result=db_open_v2(SMS_DB_PATH,flag,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    thiz->priv_data = TG_CALLOC_V2(sizeof(SmsProvider));
    sub_provider=(SmsProvider*)thiz->priv_data ;
    sub_provider->db_handle=db_handle;
    return result;
}


static INT32 sms_provider_close(ContentProvider* thiz)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    INT32 result=db_close(sub_provider->db_handle);
    if (result == DB_OK)
        TG_FREE(thiz->priv_data);
    return result;
}
static INT32 sms_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    INT32 db_ret=DB_OK;
    INT32 count=0;
    INT32 msg_type = SMS_MESSAGE_TYPE_INBOX;
    DatabaseHandle* db_handle = NULL;
    const CHAR* table = SMS_TABLE_NAME;
    if(dest==SMS_PROVIDER_DESTINATION_CONVERSATION)
   	table = SMS_THREADS_TABLE_NAME;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);
    db_ret = db_table_get_count_v2(db_handle,table,where_clause,&count);

    return db_ret==DB_OK?count:0;

}

static INT32 sms_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest)
{
    
    return sms_provider_get_count_v2(thiz,dest,NULL);

}

static INT32 sms_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num, ContentValues* values)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    const CHAR* table = SMS_TABLE_NAME;
    INT32 db_ret=DB_OK;


    DatabaseHandle* db_handle = NULL;
    ContentValues* real_values=NULL;
    const CHAR* address = NULL;
    BOOL has_col = FALSE;
    INT64 thread_id = -1;
    return_val_if_fail(sub_provider&&values&&SMS_PROVIDER_DESTINATION_LIST==dest, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);

    real_values = content_values_duplicate( (ContentValues*)values);

//address && threaqd id
    address= content_values_get_string_v2(real_values,SMS_TABLE_COL_ADDRESS);
    if (address)
    {

        db_ret=sms_get_or_create_thread_id(db_handle,address,&thread_id);
        if (db_ret==DB_OK)
        {

            content_values_put_int(real_values,SMS_TABLE_COL_THREAD_ID,thread_id);
        }
    }


    db_ret = db_insert(db_handle,table,real_values);
    content_values_destroy(real_values);
    return db_ret;

}


static Cursor* sms_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    const CHAR* table = SMS_TABLE_NAME;
    Cursor* cursor =NULL;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, NULL);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, NULL);

   if(dest==SMS_PROVIDER_DESTINATION_CONVERSATION)
   	table = SMS_THREADS_TABLE_NAME;
   
    cursor = db_query(db_handle ,table,col_list,clause,order_by,limit);

    return cursor;

}

static Cursor* sms_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by, DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    const CHAR* table = SMS_TABLE_NAME;
    Cursor* cursor =NULL;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, NULL);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, NULL);
    if(dest==SMS_PROVIDER_DESTINATION_CONVERSATION)
   	table = SMS_THREADS_TABLE_NAME;

    cursor =  db_query_v2(db_handle ,table,col_list,clause,group_by,having,order_by,limit);

    return cursor;

}

static INT32 sms_provider_update(ContentProvider* thiz,ContentProviderDestination dest, ContentValues* values, DatabaseWhereClause* filter)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    const CHAR* table = SMS_TABLE_NAME;
    INT32 db_ret=DB_OK;

    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);
     if(SMS_PROVIDER_DESTINATION_LIST==dest)
	 table = SMS_THREADS_TABLE_NAME;
    db_ret =  db_update(db_handle ,table,values,filter);
    return db_ret;

}

static INT32 sms_provider_delete(ContentProvider* thiz,ContentProviderDestination dest,DatabaseWhereClause* clause)
{

    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;

    INT32 db_ret=DB_OK;
    DatabaseHandle* db_handle = NULL;

    return_val_if_fail(sub_provider&&SMS_PROVIDER_DESTINATION_LIST==dest, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);

    db_ret = db_delete(db_handle ,SMS_TABLE_NAME,clause);

    return db_ret;

}
static INT32 sms_provider_last_error(ContentProvider* thiz)
{
    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, DB_ERROR);
    db_handle = sub_provider->db_handle;
    return_val_if_fail(db_handle, DB_ERROR);
    return db_last_error(db_handle);

}
static DatabaseHandle* sms_provider_get_db_handle(ContentProvider* thiz)
{
    SmsProvider* sub_provider =(SmsProvider*) thiz->priv_data;
    DatabaseHandle* db_handle = NULL;
    return_val_if_fail(sub_provider, NULL);
    db_handle = sub_provider->db_handle;

    return db_handle;

}
static INT32 sms_provider_release_db_handle(ContentProvider* thiz)
{

    return DB_OK;

}

ContentProviderProcess g_sms_procs = {
    sms_provider_get_id,
    sms_provider_create,
    sms_provider_open,
    sms_provider_close,

    sms_provider_insert,
    sms_provider_query,
    sms_provider_query_v2,
    sms_provider_update,
    sms_provider_delete,

    sms_provider_get_count,
    sms_provider_get_count_v2,

    sms_provider_last_error,

    sms_provider_get_db_handle,
    sms_provider_release_db_handle,
};


