
#include "content_provider.h"

#include "calllog_provider.h"
#include "content_provider_internal.h"
#include "contact_provider_base.h"
typedef struct
{
 //   DatabaseHandle* db_handle;
 	INT32 a;   //temp
}CalllogProvider;


/*
1.
schema:
"CREATE TABLE calllog_tb (number TEXT NOT NULL, sim INTEGER DEFAULT 0, type INTEGER DEFAULT 0, duration INTEGER DEFAULT 0,time INTEGER NOT NULL,PRIMARY KEY(time DESC,number));"
no other index

first step      3902 us
first step       4068 us
first step       3612 us
all      76478 us
all     75368 us
*/
#define CALLLOG_DB_PATH "/nvram/contacts.db"



#define CALLLOG_TABLE_NAME "calllog_tb"
static const CHAR *calllog_schema = "CREATE TABLE calllog_tb (number TEXT NOT NULL, sim INTEGER DEFAULT 0, type INTEGER DEFAULT 0, duration INTEGER DEFAULT 0,time INTEGER NOT NULL,PRIMARY KEY(time DESC,number));";


//
#define CALLLOG_CALLLOG_INDEX_NAME "calllog_index"
static const CHAR *calllog_index_schema = "CREATE INDEX IF NOT EXISTS calllog_index On calllog_tb(number,time);";

static INT32 _calllog_provider_create_table_impl(DatabaseHandle* db_handle,const char* tb_name,const CHAR* schema_str,BOOL force_new_table)
{

    
    if (!force_new_table && db_table_is_exist(db_handle,tb_name))
        return DB_OK;

    if (force_new_table)
    {
        db_table_drop(db_handle,tb_name);
    }

    return db_execSQL(db_handle,(const char*)schema_str);
}

static INT32 _calllog_provider_create_tables(DatabaseHandle* db_handle)
{

    INT32 result = DB_OK;
    //call log list
    result=_calllog_provider_create_table_impl(db_handle,CALLLOG_TABLE_NAME,calllog_schema,FALSE);

    //call log  view
    // result=_calllog_provider_create_table_impl(db_handle,CALLLOG_CALLLOG_VIEW_NAME,calllog_view_schema,FALSE);

    //  call log  index
    //result=_calllog_provider_create_table_impl(db_handle,CALLLOG_CALLLOG_INDEX_NAME,calllog_index_schema,FALSE);
    return result;

   

}


static const CHAR* calllog_provider_get_id()
{
    return CALLLOG_PROVIDER_ID;
}

static INT32 calllog_provider_create()
{
    DatabaseHandle* db_handle = NULL;
    INT32 result = DB_OK;
    contact_base_provider_create();
    result=db_open(CALLLOG_DB_PATH,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    result=_calllog_provider_create_tables(db_handle);
    result=db_close(db_handle);
    return result;
}

static INT32 calllog_provider_open(ContentProvider* thiz)
{
    
    INT32 result = DB_OK;
    
    result=contact_base_provider_open();
    //result=db_open_v2(CALLLOG_DB_PATH, flag,&db_handle);
    return_val_if_fail(result==DB_OK, result);
    thiz->priv_data = TG_CALLOC_V2(sizeof(CalllogProvider));

    
    return result;
}


static INT32 calllog_provider_close(ContentProvider* thiz)
{

    INT32 result=contact_base_provider_close();
    if(result == DB_OK)
	TG_FREE(thiz->priv_data);
    return result;
}

static INT32 calllog_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause)
{

    CalllogProvider* sub_provider =(CalllogProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    INT32 db_ret=DB_OK;
    INT32 count=0;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);
 

    switch (dest)
    {
    case CALLLOG_PROVIDER_DESTINATION_LIST:
        table = CALLLOG_TABLE_NAME;
        break;
    default:
        table = CALLLOG_TABLE_NAME;
        break;
    }
    db_ret = db_table_get_count_v2(db_handle,table,where_clause,&count);
    contact_base_provider_release_db_handle();
    return_val_if_fail(db_ret==DB_OK, 0);
    return count;

}

static INT32 calllog_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest)
{

    return calllog_provider_get_count_v2(thiz,dest,NULL);

}


static INT32 calllog_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num, ContentValues* values)
{

    CalllogProvider* sub_provider =(CalllogProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    INT32 db_ret=DB_OK;
    
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);
    

    switch (dest)
    {
    case CALLLOG_PROVIDER_DESTINATION_LIST:
        table = CALLLOG_TABLE_NAME;
        break;
    default:
        table = CALLLOG_TABLE_NAME;
        break;
    }
    db_ret = db_insert(db_handle,table,values);
    contact_base_provider_release_db_handle();
    return db_ret;

}


static Cursor* calllog_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{

    CalllogProvider* sub_provider =(CalllogProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    Cursor* cursor =NULL;

    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(sub_provider&&db_handle, NULL);


    switch (dest)
    {
    case CALLLOG_PROVIDER_DESTINATION_LIST:
	//table="calllog_tb log LEFT JOIN (SELECT me.ID,name,ext.value AS phonenumber FROM contacts_extension_tb_number ext INNER JOIN contacts_me_tb me ON me.ID=ext.ID UNION SELECT ID,name,phonenumber FROM contacts_sim_tb) tmp_tb ON tmp_tb.phonenumber=number";
	table="(SELECT ext.ID,log.* FROM calllog_tb log LEFT JOIN contacts_extension_tb_number ext ON log.number=ext.value   ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";	
	//table="(SELECT ext.ID,log.* FROM calllog_tb log LEFT JOIN contacts_extension_tb_number ext ON log.number=ext.value  GROUP BY log.number ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";	
	break;
    default:
	 return_val_if_fail(0, NULL);
        break;
    }
    cursor = db_query(db_handle ,table,col_list,clause,order_by,limit);
    contact_base_provider_release_db_handle();
     return cursor;

}

static Cursor* calllog_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by, DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{

    CalllogProvider* sub_provider =(CalllogProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    Cursor* cursor =NULL;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(sub_provider&&db_handle, NULL);
    

    switch (dest)
    {
    case CALLLOG_PROVIDER_DESTINATION_LIST:
	//query both sim and me
	//table="calllog_tb log LEFT JOIN (SELECT me.ID,name,ext.value AS phonenumber FROM contacts_extension_tb_number ext INNER JOIN contacts_me_tb me ON me.ID=ext.ID UNION SELECT ID,name,phonenumber FROM contacts_sim_tb) tmp_tb ON tmp_tb.phonenumber=number";
	table="(SELECT ext.ID,log.* FROM calllog_tb log LEFT JOIN contacts_extension_tb_number ext ON log.number=ext.value   ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";	
	//table="(SELECT ext.ID,log.* FROM calllog_tb log LEFT JOIN contacts_extension_tb_number ext ON log.number=ext.value  GROUP BY log.number ) tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID";	
        break;
    default:
	 return_val_if_fail(0, NULL);
        break;
    }

    cursor =  db_query_v2(db_handle ,table,col_list,clause,group_by,having,order_by,limit);
    contact_base_provider_release_db_handle();
     return cursor;

}

static INT32 calllog_provider_update(ContentProvider* thiz,ContentProviderDestination dest, ContentValues* values, DatabaseWhereClause* filter)
{

    CalllogProvider* sub_provider =(CalllogProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    INT32 db_ret=DB_OK;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);

    switch (dest)
    {
    case CALLLOG_PROVIDER_DESTINATION_LIST:
        table = CALLLOG_TABLE_NAME;
        break;
    default:
        table = CALLLOG_TABLE_NAME;
        break;
    }
    db_ret =  db_update(db_handle ,table,values,filter);
    contact_base_provider_release_db_handle();
     return db_ret;

}

static INT32 calllog_provider_delete(ContentProvider* thiz,ContentProviderDestination dest,DatabaseWhereClause* filter)
{

    CalllogProvider* sub_provider =(CalllogProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    INT32 db_ret=DB_OK;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);


    switch (dest)
    {
    case CALLLOG_PROVIDER_DESTINATION_LIST:
        table = CALLLOG_TABLE_NAME;
        break;
    default:
        table = CALLLOG_TABLE_NAME;
        break;
    }
     db_ret = db_delete(db_handle ,table,filter);
    contact_base_provider_release_db_handle();
     return db_ret;

}
static INT32 calllog_provider_last_error(ContentProvider* thiz)
{
    
    return contact_base_provider_get_last_error();

}
static DatabaseHandle* calllog_provider_get_db_handle(ContentProvider* thiz)
{
    
    return contact_base_provider_obtain_db_handle();

}
static INT32 calllog_provider_release_db_handle(ContentProvider* thiz)
{
    
    return contact_base_provider_release_db_handle();

}

ContentProviderProcess g_calllog_procs = {
    calllog_provider_get_id,
    calllog_provider_create,
    calllog_provider_open,
    calllog_provider_close,

    calllog_provider_insert,
    calllog_provider_query,
    calllog_provider_query_v2,
    calllog_provider_update,
    calllog_provider_delete,

    calllog_provider_get_count,
    calllog_provider_get_count_v2,

   calllog_provider_last_error,

   calllog_provider_get_db_handle,
    calllog_provider_release_db_handle,
};


