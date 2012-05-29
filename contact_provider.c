
#include "content_provider.h"
#include "content_provider_internal.h"
#include "contact_provider.h"
#include "contact_provider_base.h"
#include "db_util.h"
typedef struct
{
//   DatabaseHandle* db_handle;
    INT32 a;
}ContactProvider;


typedef INT32 (*CONTACT_PROVIDER_ADD_DEFAULT_RECORD)(DatabaseHandle* db_handle);

#define CONTACT_DB_PATH "/nvram/contacts.db"

#define CONTACT_ME_TABLE_NAME "contacts_me_tb"
#define CONTACT_EXTENSION_TABLE_NUMBER_NAME "contacts_extension_tb_number"
#define CONTACT_EXTENSION_TABLE_OTHER_NAME "contacts_extension_tb_other"
#define CONTACT_TAG_TABLE_NAME "contacts_tag_tb"
#define CONTACT_SIM_TABLE_NAME  "contacts_sim_tb"

#define CONTACT_ME_CONTACTS_TABLE_NAME "contacts_me_tb"
#define CONTACT_ALL_VIEW_NAME "contacts_all_view"
static const CHAR *s_contacts_me_schema_str =
    "create table contacts_me_tb (ID INTEGER PRIMARY KEY autoincrement, group_id INTEGER DEFAULT 1,main_num_tag INTEGER DEFAULT 0, name TEXT DEFAULT NULL,key_map_str TEXT DEFAULT NULL,\
    FOREIGN KEY(group_id) REFERENCES contacts_me_group_tb(ID));";

static const CHAR *s_contacts_extension_number_schema_str =
    "create table contacts_extension_tb_number (ID INTEGER, tag_id INTEGER DEFAULT 0, value TEXT DEFAULT NULL,PRIMARY KEY(ID,tag_id) FOREIGN KEY(ID) REFERENCES contacts_me_tb(ID) FOREIGN KEY(tag_id) REFERENCES contacts_tag_tb(ID));";

static const CHAR *s_contacts_extension_other_schema_str =
    "create table contacts_extension_tb_other (ID INTEGER, tag_id INTEGER DEFAULT 0, value TEXT DEFAULT NULL,PRIMARY KEY(ID,tag_id) FOREIGN KEY(ID) REFERENCES contacts_me_tb(ID) FOREIGN KEY(tag_id) REFERENCES contacts_tag_tb(ID));";

static const CHAR *s_contacts_tag_schema_str =
    "create table contacts_tag_tb (ID INTEGER PRIMARY KEY, class_id INTEGER DEFAULT 0, name TEXT DEFAULT NULL);";
static const CHAR *s_contacts_sim_schema_str =
    "create table contacts_sim_tb (ID INTEGER PRIMARY KEY, name TEXT DEFAULT NULL, phonenumber TEXT DEFAULT NULL, key_map_str TEXT DEFAULT NULL);";
#define CONTACT_ME_GROUP_TABLE_NAME "contacts_me_group_tb"
#define CONTACT_ME_GROUP_GROUP_COL_NAME "name"
#define CONTACT_ME_GROUP_GROUP_COL_AUDIO "audio"
static const char *contacts_me_group_schema_str = "create table contacts_me_group_tb (ID INTEGER PRIMARY KEY, name TEXT NOT NULL, audio TEXT DEFAULT NULL);";

#define CONTACT_ME_INDEX_NAME "contact_index"
static const CHAR *contact_index_schema = "CREATE INDEX IF NOT EXISTS contact_index On contacts_me_tb(name COLLATE NOCASE);";

#define CONTACT_ME_NUMBER_INDEX "contacts_me_num_index"
static const CHAR *contact_me_num_index_schema = "CREATE INDEX IF NOT EXISTS contacts_me_num_index On contacts_extension_tb_number(value);";

static const CHAR *s_contacts_view_all_schema_str =
    "create view IF NOT EXISTS contacts_all_view as select ID, name from contacts_me_tb;";

//
static INT32 _contact_provider_add_default_groups(DatabaseHandle* db_handle);


static INT32 _contact_provider_create_table_impl(DatabaseHandle* db_handle,const char* tb_name,const CHAR* schema_str,BOOL force_new_table, CONTACT_PROVIDER_ADD_DEFAULT_RECORD cb)
{
    INT32 result = DB_OK;

    if (!force_new_table && db_table_is_exist(db_handle,tb_name))
        return DB_OK;

    if (force_new_table)
    {
        db_table_drop(db_handle,tb_name);
    }

    result=db_execSQL(db_handle,(const char*)schema_str);
    if (result == DB_OK && cb!=NULL)
    {
        result=cb(db_handle);
    }

    return result;
}

static INT32 _contact_provider_create_index_impl(DatabaseHandle* db_handle,const char* tb_name,const CHAR* schema_str,BOOL force_new_table)
{
    INT32 result = DB_OK;

    if (force_new_table)
    {
        db_index_drop(db_handle,tb_name);
    }

    result=db_execSQL(db_handle,(const char*)schema_str);

    return result;
}

static INT32 _contact_provider_create_view_impl(DatabaseHandle* db_handle,const char* tb_name,const CHAR* schema_str,BOOL force_new_table)
{
    INT32 result = DB_OK;

    if (force_new_table)
    {
        db_view_drop(db_handle,tb_name);
    }

    result=db_execSQL(db_handle,(const char*)schema_str);

    return result;
}


static INT32 _contact_provider_create_tables(DatabaseHandle* db_handle)
{

    INT32 result = DB_OK;


    //me people
    result=_contact_provider_create_table_impl(db_handle,CONTACT_ME_TABLE_NAME,s_contacts_me_schema_str,FALSE,NULL);


    //me group
    result=_contact_provider_create_table_impl(db_handle,CONTACT_ME_GROUP_TABLE_NAME,contacts_me_group_schema_str,FALSE,_contact_provider_add_default_groups);

    result=_contact_provider_create_table_impl(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,s_contacts_extension_number_schema_str,FALSE,NULL);
    result=_contact_provider_create_table_impl(db_handle,CONTACT_EXTENSION_TABLE_OTHER_NAME,s_contacts_extension_other_schema_str,FALSE,NULL);
    result=_contact_provider_create_table_impl(db_handle,CONTACT_TAG_TABLE_NAME,s_contacts_tag_schema_str,FALSE,NULL);
    result=_contact_provider_create_table_impl(db_handle,CONTACT_SIM_TABLE_NAME,s_contacts_sim_schema_str,FALSE,NULL);
    //me index
    result=_contact_provider_create_index_impl(db_handle,CONTACT_ME_INDEX_NAME,contact_index_schema,TRUE);
    result=_contact_provider_create_index_impl(db_handle,CONTACT_ME_NUMBER_INDEX,contact_me_num_index_schema,FALSE);


    result=_contact_provider_create_view_impl(db_handle,CONTACT_ALL_VIEW_NAME,s_contacts_view_all_schema_str,TRUE);

    return result;


}

//default group list
static INT32 _contact_provider_add_default_groups(DatabaseHandle* db_handle)
{


    ContentValues* values=content_values_create();
    INT32  num = 0;
    INT32 i = 0;
    INT32 db_ret=DB_OK;
    DatabaseStatement* stmt = NULL;
    static CHAR* s_contact_group_name[]={"Blacklist","Frequent","Colleague","Classmate","Family","Other"};
    num= sizeof(s_contact_group_name)/sizeof(s_contact_group_name[0]);
    return_val_if_fail(values, DB_ERROR);
    stmt = db_stmt_build_insert_statement(db_handle,CONTACT_ME_GROUP_TABLE_NAME,CONTACT_ME_GROUP_GROUP_COL_NAME,1);
    if (!stmt)
    {
        content_values_destroy(values);
        return DB_ERROR;
    }
    for (i=0;i<num;i++)
    {
        content_values_put_string(values,CONTACT_ME_GROUP_GROUP_COL_NAME,s_contact_group_name[i]);
        db_ret = db_stmt_bind_content_values( stmt,values);
        if (db_ret == DB_OK)
        {
            db_ret=db_stmt_step(stmt);
            if (db_ret!=DB_OK)
            {
                break;
            }
        }

    }
    db_stmt_finalize(stmt);
    content_values_destroy(values);
    return db_ret;

}

static const CHAR* contact_provider_get_id()
{
    return "contact";
}

static INT32 contact_provider_create()
{
    DatabaseHandle* db_handle = NULL;
    INT32 result = DB_OK;
    contact_base_provider_create();
    result=db_open(CONTACT_DB_PATH,&db_handle);
    return_val_if_fail(result==DB_OK, result);

    result=_contact_provider_create_tables(db_handle);
    result=db_close(db_handle);
    return result;
}

static INT32 contact_provider_open(ContentProvider* thiz)
{

    // ContactProvider* sub_provider = NULL;
    INT32 result = DB_OK;


    result=contact_base_provider_open();
    return_val_if_fail(result==DB_OK, result);
    thiz->priv_data = TG_CALLOC_V2(sizeof(ContactProvider));
    // sub_provider=(ContactProvider*)thiz->priv_data;
    //  sub_provider->db_handle=db_handle;
    return result;
}


static INT32 contact_provider_close(ContentProvider* thiz)
{

    //ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    INT32 result=contact_base_provider_close();
    if (result == DB_OK)
        TG_FREE(thiz->priv_data);
    return result;
}


static INT32 contact_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause)
{

    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    INT32 db_ret=DB_OK;
    INT32 count = 0;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);


    switch (dest)
    {
    case CONTACT_PROVIDER_DESTINATION_CONTACTS:
        table = CONTACT_ME_CONTACTS_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_GROUP:
        table = CONTACT_ME_GROUP_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_SIM:
        table = CONTACT_SIM_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_VIEW_ALL:
        table = CONTACT_ALL_VIEW_NAME;
        break;
    }
    db_ret=db_table_get_count_v2(db_handle, table,where_clause, &count);
    contact_base_provider_release_db_handle();
    return_val_if_fail(db_ret==DB_OK, 0);

    return count;

}

static INT32 contact_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest)
{

    return contact_provider_get_count_v2(thiz,dest,NULL);
}


Cursor* contact_provider_search_in_temp_view(ContentProvider* thiz,const CHAR* str )
{
    INT32 result;
    Cursor* cursor;
    WCHAR* name;
    CHAR* number;
    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, NULL);

    result=_contact_provider_create_table_impl(db_handle,"contacts_temp_view",str,FALSE,NULL);
    cursor = db_query_v2(db_handle ,"contacts_temp_view","ID,name,phonenumber",NULL,NULL,NULL,NULL,NULL);
    do{
        name = (WCHAR*)cursor_get_string_16(cursor,1);
        number = (CHAR*)cursor_get_string(cursor,2);
    }while (cursor_move_to_next(cursor));
    db_view_drop(db_handle,"contacts_temp_view");
    contact_base_provider_release_db_handle();
    return cursor;
}


static INT32 contact_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num, ContentValues* values)
{

    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    ContentValuesItor itor;
    ContentValues *values_me, *values_ext;
    CHAR* values_ret = NULL;
    INT64 ID;
    INT32 db_ret=DB_OK;
    DatabaseStatement* stmt = NULL;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    UINT32 t1 = db_get_ticks();
    UINT32 t2 = 0;
    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);




    switch (dest)
    {
    case CONTACT_PROVIDER_DESTINATION_CONTACTS:
        table = CONTACT_ME_CONTACTS_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_GROUP:
        table = CONTACT_ME_GROUP_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_NUMBER:
        table = CONTACT_EXTENSION_TABLE_NUMBER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_OTHER:
        table = CONTACT_EXTENSION_TABLE_OTHER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_SIM:
        table = CONTACT_SIM_TABLE_NAME;
        break;
    }
    if (CONTACT_PROVIDER_DESTINATION_CONTACTS==dest)
    {
        INT64 value_group_id,value_main_num_tag;
        WCHAR* str_name=NULL;
        values_me = content_values_create();
        content_values_get_int(values,"group_id",&value_group_id);
        content_values_delete_key(values,"group_id");
        content_values_put_int(values_me,"group_id",value_group_id);
        content_values_get_int(values,"main_num_tag",&value_main_num_tag);
        content_values_delete_key(values,"main_num_tag");
        content_values_put_int(values_me,"main_num_tag",value_main_num_tag);
        content_values_get_string16(values,"name",&str_name);
        content_values_delete_key(values,"name");
        content_values_put_string_16(values_me,"name",str_name);
        db_ret = db_insert(db_handle,table,values_me);
        content_values_destroy(values_me);
        TG_FREE(str_name);
        ID = db_last_insert_rowid(db_handle);
        for (itor=content_values_first(values);itor;itor=content_values_next(values,itor))
        {
            const CHAR* key = content_values_get_key(itor);
            content_values_get_string(values,key,&values_ret);
            values_ext = content_values_create();
            if (0 == strcmp(key,"mobile"))
            {
                content_values_put_int(values_ext,"ID",ID);
                content_values_put_int(values_ext,"tag_id",CONTACT_EXTENSION_TAG_MOBILE);
                content_values_put_string(values_ext,"value",values_ret);
                db_ret |= db_insert(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,values_ext);
            }
            else if (0 == strcmp(key,"home"))
            {
                content_values_put_int(values_ext,"ID",ID);
                content_values_put_int(values_ext,"tag_id",CONTACT_EXTENSION_TAG_HOME);
                content_values_put_string(values_ext,"value",values_ret);
                db_ret |= db_insert(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,values_ext);
            }
            else if (0 == strcmp(key,"office"))
            {
                content_values_put_int(values_ext,"ID",ID);
                content_values_put_int(values_ext,"tag_id",CONTACT_EXTENSION_TAG_OFFICE);
                content_values_put_string(values_ext,"value",values_ret);
                db_ret |= db_insert(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,values_ext);
            }
            else if (0 == strcmp(key,"email"))
            {
                content_values_put_int(values_ext,"ID",ID);
                content_values_put_int(values_ext,"tag_id",CONTACT_EXTENSION_TAG_EMAIL);
                content_values_put_string(values_ext,"value",values_ret);
                db_ret |= db_insert(db_handle,CONTACT_EXTENSION_TABLE_OTHER_NAME,values_ext);
            }
            content_values_destroy(values_ext);
            TG_FREE(values_ret);
        }
        if (DB_OK == db_ret)
        {
            db_ret = (INT32)ID;
        }
        else
        {
            db_ret = -1;
        }
    }
    else
    {
        stmt = db_stmt_build_insert_statement(db_handle,table,col_list,col_num);
        return_val_if_fail(stmt, DB_ERROR);
        db_ret= db_stmt_bind_content_values( stmt,(ContentValues*)values);
        if (db_ret==DB_OK)
            db_ret=db_stmt_step(stmt);
        db_stmt_finalize(stmt);
    }
    contact_base_provider_release_db_handle();

    t2 = db_get_ticks();
    DB_PRINTF("contact_provider_insert use %d\n",t2-t1);


    return db_ret;

}

static Cursor* contact_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{

    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    Cursor* cursor = NULL;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();
    return_val_if_fail(sub_provider&&db_handle, NULL);

    switch (dest)
    {
    case CONTACT_PROVIDER_DESTINATION_CONTACTS:
        table = CONTACT_ME_CONTACTS_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_SIM:
        table = CONTACT_SIM_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_GROUP:
        table = CONTACT_ME_GROUP_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_VIEW_ALL:
        table = CONTACT_ALL_VIEW_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_NUMBER:
        table = CONTACT_EXTENSION_TABLE_NUMBER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_OTHER:
        table = CONTACT_EXTENSION_TABLE_OTHER_NAME;
        break;
    }
    cursor= db_query(db_handle ,table,col_list,clause,order_by,limit);
    contact_base_provider_release_db_handle();
    return cursor;

}

static Cursor* contact_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{

    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    Cursor* cursor = NULL;
    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(sub_provider&&db_handle, NULL);


    switch (dest)
    {
    case CONTACT_PROVIDER_DESTINATION_CONTACTS:
        table = CONTACT_ME_CONTACTS_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_SIM:
        table = CONTACT_SIM_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_GROUP:
        table = CONTACT_ME_GROUP_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_VIEW_ALL:
        table = CONTACT_ALL_VIEW_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_NUMBER:
        table = CONTACT_EXTENSION_TABLE_NUMBER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_OTHER:
        table = CONTACT_EXTENSION_TABLE_OTHER_NAME;
        break;
    }
    cursor = db_query_v2(db_handle ,table,col_list,clause,group_by,having,order_by,limit);
    contact_base_provider_release_db_handle();
    return cursor;
}

static INT32 contact_provider_update(ContentProvider* thiz,ContentProviderDestination dest, ContentValues* values, DatabaseWhereClause* filter)
{

    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    ContentValuesItor itor;
    ContentValues *values_me, *values_ext;
    CHAR* values_ret = NULL;
    INT32 db_ret=DB_OK;
    CHAR* statement;
    CHAR statement_ext[50];
    DatabaseWhereClause* clause;

    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);

    switch (dest)
    {
    case CONTACT_PROVIDER_DESTINATION_CONTACTS:
        table = CONTACT_ME_CONTACTS_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_GROUP:
        table = CONTACT_ME_GROUP_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_NUMBER:
        table = CONTACT_EXTENSION_TABLE_NUMBER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_OTHER:
        table = CONTACT_EXTENSION_TABLE_OTHER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_SIM:
        table = CONTACT_SIM_TABLE_NAME;
        break;
    }

    if (CONTACT_PROVIDER_DESTINATION_CONTACTS==dest)
    {
        INT64 value_group_id,value_main_num_tag;
        WCHAR* str_name=NULL;
        values_me = content_values_create();
        content_values_get_int(values,"group_id",&value_group_id);
        content_values_delete_key(values,"group_id");
        content_values_put_int(values_me,"group_id",value_group_id);
        content_values_get_int(values,"main_num_tag",&value_main_num_tag);
        content_values_delete_key(values,"main_num_tag");
        content_values_put_int(values_me,"main_num_tag",value_main_num_tag);
        content_values_get_string16(values,"name",&str_name);
        content_values_delete_key(values,"name");
        content_values_put_string_16(values_me,"name",str_name);

        db_update(db_handle,table,values_me,filter);
        content_values_destroy(values_me);
        TG_FREE(str_name);

        for (itor=content_values_first(values);itor;itor=content_values_next(values,itor))
        {
            const CHAR* key = content_values_get_key(itor);
            content_values_get_string(values,key,&values_ret);
            values_ext = content_values_create();
            if (0 == strcmp(key,"mobile"))
            {
                content_values_put_string(values_ext,"value",values_ret);
                statement = (CHAR*)db_clause_get_statement(filter);
                sprintf(statement_ext,"%s AND tag_id=%d",statement,CONTACT_EXTENSION_TAG_MOBILE);
                clause = db_clause_create(DB_WHERE_CLAUSE,statement_ext,0);
                db_update(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,values_ext,clause);
                db_clause_destroy(clause);
            }
            else if (0 == strcmp(key,"home"))
            {
                content_values_put_string(values_ext,"value",values_ret);
                statement = (CHAR*)db_clause_get_statement(filter);
                sprintf(statement_ext,"%s AND tag_id=%d",statement,CONTACT_EXTENSION_TAG_HOME);
                clause = db_clause_create(DB_WHERE_CLAUSE,statement_ext,0);
                db_update(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,values_ext,clause);
                db_clause_destroy(clause);
            }
            else if (0 == strcmp(key,"office"))
            {
                content_values_put_string(values_ext,"value",values_ret);
                statement = (CHAR*)db_clause_get_statement(filter);
                sprintf(statement_ext,"%s AND tag_id=%d",statement,CONTACT_EXTENSION_TAG_OFFICE);
                clause = db_clause_create(DB_WHERE_CLAUSE,statement_ext,0);
                db_update(db_handle,CONTACT_EXTENSION_TABLE_NUMBER_NAME,values_ext,clause);
                db_clause_destroy(clause);
            }
            else if (0 == strcmp(key,"email"))
            {
                content_values_put_string(values_ext,"value",values_ret);
                statement = (CHAR*)db_clause_get_statement(filter);
                sprintf(statement_ext,"%s AND tag_id=%d",statement,CONTACT_EXTENSION_TAG_EMAIL);
                clause = db_clause_create(DB_WHERE_CLAUSE,statement_ext,0);
                db_update(db_handle,CONTACT_EXTENSION_TABLE_OTHER_NAME,values_ext,clause);
                db_clause_destroy(clause);
            }
            content_values_destroy(values_ext);
            TG_FREE(values_ret);
        }
    }

    else
    {
        db_ret  =  db_update(db_handle ,table,values,filter);
    }
    contact_base_provider_release_db_handle();
    return db_ret;

}

static INT32 contact_provider_delete(ContentProvider* thiz,ContentProviderDestination dest, DatabaseWhereClause* filter)
{

    ContactProvider* sub_provider =(ContactProvider*) thiz->priv_data;
    const CHAR* table = NULL;
    INT32 db_ret=DB_OK;

    DatabaseHandle* db_handle = contact_base_provider_obtain_db_handle();

    return_val_if_fail(sub_provider&&db_handle, DB_ERROR);

    switch (dest)
    {
    case CONTACT_PROVIDER_DESTINATION_CONTACTS:
        table = CONTACT_ME_CONTACTS_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_SIM:
        table = CONTACT_SIM_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_GROUP:
        table = CONTACT_ME_GROUP_TABLE_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_NUMBER:
        table = CONTACT_EXTENSION_TABLE_NUMBER_NAME;
        break;
    case CONTACT_PROVIDER_DESTINATION_EXTENSION_OTHER:
        table = CONTACT_EXTENSION_TABLE_OTHER_NAME;
        break;
    }
    db_ret = db_delete(db_handle ,table,filter);
    contact_base_provider_release_db_handle();
    return db_ret;

}

static INT32 contact_provider_last_error(ContentProvider* thiz)
{

    return contact_base_provider_get_last_error();

}
static DatabaseHandle* contact_provider_get_db_handle(ContentProvider* thiz)
{

    return contact_base_provider_obtain_db_handle();
}

static INT32 contact_provider_release_db_handle(ContentProvider* thiz)
{

    return contact_base_provider_release_db_handle();

}

ContentProviderProcess g_contact_procs = {
    contact_provider_get_id,
    contact_provider_create,
    contact_provider_open,
    contact_provider_close,


    contact_provider_insert,
    contact_provider_query,
    contact_provider_query_v2,
    contact_provider_update,
    contact_provider_delete,

    contact_provider_get_count,
    contact_provider_get_count_v2,

    contact_provider_last_error,

    contact_provider_get_db_handle,
    contact_provider_release_db_handle,
};


