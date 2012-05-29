#include "content_provider.h"

#include "sms_provider.h"
#include "content_provider_internal.h"
#include "contact_provider_base.h"

#if 0
typedef struct
{
    //DatabaseHandle* db_handle;
    INT32 a;   //temp
}SmsProvider;

#endif


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
    contact_base_provider_create();
    result=db_open(CONTACT_DB_PATH,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    result=_sms_provider_create_tables(db_handle);
    result=db_close(db_handle);
    return result;
}
static INT32 sms_provider_open(ContentProvider* thiz)
{


    INT32 result = DB_OK;

    result=contact_base_provider_open();
    //result=db_open_v2(CALLLOG_DB_PATH, flag,&db_handle);
    return_val_if_fail(result==DB_OK, result);

    return result;
}

static INT32 sms_provider_close(ContentProvider* thiz)
{
    return contact_base_provider_close();
}

static INT32 sms_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause)
{


    const CHAR* table = SMS_TABLE_NAME;
    INT32 db_ret=DB_OK;
    INT32 count=0;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(db_handle, DB_ERROR);
    if (dest==SMS_PROVIDER_DESTINATION_CONVERSATION)
        table = SMS_THREADS_TABLE_NAME;
    db_ret = db_table_get_count_v2(db_handle,table,where_clause,&count);
    contact_base_provider_release_db_handle();
    return db_ret==DB_OK?count:0;

}
static INT32 sms_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest)
{

    return sms_provider_get_count_v2(thiz,dest,NULL);

}

static INT32 sms_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num, ContentValues* values)
{


    const CHAR* table = SMS_TABLE_NAME;
    INT32 db_ret=DB_OK;
    ContentValues* real_values=NULL;
    const CHAR* address = NULL;
    BOOL has_col = FALSE;
    INT64 thread_id = -1;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(values&&SMS_PROVIDER_DESTINATION_LIST==dest, DB_ERROR);
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
    contact_base_provider_release_db_handle();
    return db_ret;

}
extern char *strstr(char *str1, char *str2);
static BOOL _sms_is_include_name_col(const CHAR* col_list)
{
//	CHAR name_buf[20];
    CHAR tmp[5];
    INT32 len = 0;
    return_val_if_fail(col_list, FALSE);
    len = strlen(col_list);
    return_val_if_fail(len>=4, FALSE);
    memcpy(tmp,col_list,4);
    tmp[4]=0;
    //return TRUE;
    if (strcmp(tmp,"name")==0)
    {
        return TRUE;
    }
    else
    {
        return strstr (col_list, ",name")!=NULL;
    }
}

static Cursor* sms_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{

    const CHAR* table = SMS_TABLE_NAME;
    Cursor* cursor =NULL;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(db_handle, NULL);

    if (!_sms_is_include_name_col(col_list))
    {
        if (dest==SMS_PROVIDER_DESTINATION_CONVERSATION)
            table = SMS_THREADS_TABLE_NAME;


    }
    else
    {
        switch (dest)
        {
        case SMS_PROVIDER_DESTINATION_LIST:
            table="(SELECT ext.ID ,sms_tb.* FROM sms_tb LEFT JOIN contacts_extension_tb_number ext ON sms_tb.address=ext.value ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";
            break;
        case SMS_PROVIDER_DESTINATION_CONVERSATION:
            table="(SELECT ext.ID,threads_tb.* FROM threads_tb LEFT JOIN contacts_extension_tb_number ext ON threads_tb.canonical_address=ext.value ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";
            break;
        default:
            return_val_if_fail(0, NULL);
            break;
        }
    }
    cursor = db_query(db_handle ,table,col_list,clause,order_by,limit);

   
    contact_base_provider_release_db_handle();
    return cursor;

}

static Cursor* sms_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by, DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{


    const CHAR* table = SMS_TABLE_NAME;
    Cursor* cursor =NULL;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(db_handle, NULL);
    if (!_sms_is_include_name_col(col_list))
    {
        if (dest==SMS_PROVIDER_DESTINATION_CONVERSATION)
            table = SMS_THREADS_TABLE_NAME;


    }
    else
    {
        switch (dest)
        {
        case SMS_PROVIDER_DESTINATION_LIST:
            table="(SELECT ext.ID ,sms_tb.* FROM sms_tb LEFT JOIN contacts_extension_tb_number ext ON sms_tb.address=ext.value ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";
            break;
        case SMS_PROVIDER_DESTINATION_CONVERSATION:
            table="(SELECT ext.ID,threads_tb.* FROM threads_tb LEFT JOIN contacts_extension_tb_number ext ON threads_tb.canonical_address=ext.value ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";
            break;
        default:
            return_val_if_fail(0, NULL);
            break;
        }
    }
    cursor =  db_query_v2(db_handle ,table,col_list,clause,group_by,having,order_by,limit);
    contact_base_provider_release_db_handle();
    return cursor;

}

static INT32 sms_provider_update(ContentProvider* thiz,ContentProviderDestination dest, ContentValues* values, DatabaseWhereClause* filter)
{


    const CHAR* table = SMS_TABLE_NAME;
    INT32 db_ret=DB_OK;

    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(db_handle, DB_ERROR);
    if (SMS_PROVIDER_DESTINATION_CONVERSATION==dest)
        table = SMS_THREADS_TABLE_NAME;
    db_ret =  db_update(db_handle ,table,values,filter);
    contact_base_provider_release_db_handle();
    return db_ret;

}

static INT32 sms_provider_delete(ContentProvider* thiz,ContentProviderDestination dest,DatabaseWhereClause* clause)
{



    INT32 db_ret=DB_OK;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(SMS_PROVIDER_DESTINATION_LIST==dest, DB_ERROR);
    return_val_if_fail(db_handle, DB_ERROR);

    db_ret = db_delete(db_handle ,SMS_TABLE_NAME,clause);
    contact_base_provider_release_db_handle();
    return db_ret;

}
static INT32 sms_provider_last_error(ContentProvider* thiz)
{

    return contact_base_provider_get_last_error();

}
static DatabaseHandle* sms_provider_get_db_handle(ContentProvider* thiz)
{
    return contact_base_provider_obtain_db_handle();

}
static INT32 sms_provider_release_db_handle(ContentProvider* thiz)
{

    return contact_base_provider_release_db_handle();
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


