
#include "content_provider.h"

#include "content_provider_internal.h"
#include "contact_provider_base.h"
#include "mmssms_db_helper.h"
#include "contact_provider.h"
#include <tg_utility.h>


/*
address: phonenumber
thread_id: thread id
date	    : sms date, i am not sure if i need set the default as 0
protocol: 0 SMS_PROTO, 1 MMS_PROTO
status:
type:  default is 0(inbox)
	#define SMS_MESSAGE_TYPE_INBOX				  	0
	#define SMS_MESSAGE_TYPE_SENT				  		1
	#define SMS_MESSAGE_TYPE_OUTBOX				  	2
	#define SMS_MESSAGE_TYPE_DRAFT				  	3    			//THE DRAFT BOX must 3,or i need to change the sql statement
	#define SMS_MESSAGE_TYPE_FAILED				  	4   			//failed outgoing message
	#define SMS_MESSAGE_TYPE_QUEUED				  	5  			//for message to send later
read:      0---unread; 1--read
status:	0---complete; 64---pending; 128----failed  //android code, i delete it,
reply_path_present: TP-Reply-Path
subject:	there is no subject in sms, //android code, i delete it,
body:      sms text
error_code://android code, i delete it,

*/


static const CHAR *s_sms_schema = "CREATE TABLE sms_tb (sms_id INTEGER PRIMARY KEY,thread_id INTEGER REFERENCES threads_tb(thread_id) ,address TEXT,	\
                                  date INTEGER DEFAULT 0,protocol INTEGER DEFAULT 0,read INTEGER DEFAULT 0,type INTEGER DEFAULT 0,\
                                  reply_path_present INTEGER,body TEXT,service_center TEXT);";

static const CHAR *s_threads_schema = "CREATE TABLE threads_tb (thread_id INTEGER PRIMARY KEY autoincrement, canonical_address TEXT UNIQUE, date INTEGER DEFAULT 0, message_count INTEGER DEFAULT 0,\
                                      unread_count INTEGER DEFAULT 0, failed_count INTEGER DEFAULT 0,cached_name TEXT DEFAUT NULL)";

//some people have more than one address(phonenumber), but they need one thread id
//static const CHAR* s_canonical_addr_schema = "CREATE TABLE canonical_addr_tb (ID INTEGER PRIMARY KEY autoincrement, address TEXT,cached_name TEXT DEFAUT NULL);";



//sms in draft box is not include in the conversion list

#if 0
static const CHAR* s_threads_update_on_insert_trigger_schema = "CREATE TRIGGER IF NOT EXISTS sms_update_thread_on_insert AFTER INSERT ON sms_tb\
        BEGIN  \
        UPDATE threads_tb SET    date = (strftime('%s','now') * 1000)	\
        WHERE threads_tb.ID = new.thread_id;\
        UPDATE threads_tb SET message_count =\
        (SELECT COUNT(sms_tb.ID) FROM sms_tb  WHERE thread_id = NEW.thread_id AND sms_tb.type != 3)  WHERE threads_tb.ID = NEW.thread_id; \
        UPDATE threads_tb SET unread_count =  (SELECT COUNT(*)   FROM sms_tb  WHERE read = 0 AND type != 3  AND thread_id = NEW.thread_id) WHERE threads_tb.ID = NEW.thread_id;\
        END;";
#else
static const CHAR* s_threads_update_on_insert_trigger_schema = "CREATE TRIGGER IF NOT EXISTS sms_update_thread_on_insert AFTER INSERT ON sms_tb\
        BEGIN  \
        UPDATE threads_tb SET    date = NEW.date	\
        WHERE threads_tb.thread_id = new.thread_id;\
        UPDATE threads_tb SET message_count =\
        (SELECT COUNT(sms_tb.sms_id) FROM sms_tb  WHERE thread_id = NEW.thread_id AND sms_tb.type != 3)  WHERE threads_tb.thread_id = NEW.thread_id; \
        UPDATE threads_tb SET unread_count =  (SELECT COUNT(*)   FROM sms_tb  WHERE read = 0 AND type != 3  AND thread_id = NEW.thread_id) WHERE threads_tb.thread_id = NEW.thread_id;\
        END;";
#endif



//sms in draft box is not include in the conversion list
static const CHAR* s_threads_update_on_read_trigger_schema = "CREATE TRIGGER IF NOT EXISTS sms_update_thread_on_read_update AFTER UPDATE OF read ON sms_tb\
        BEGIN  \
        UPDATE threads_tb SET unread_count =  (SELECT COUNT(*)   FROM sms_tb  WHERE read = 0  AND type != 3 AND thread_id = NEW.thread_id) WHERE threads_tb.thread_id = NEW.thread_id;\
        END;";

//sms in draft box is not include in the conversion list
//the schema is almost the same with the insert trigger, but it is used for delete trigger, because delete trigger can not use NEW, only OLD is vaildate
static const CHAR* s_threads_update_on_delete_trigger_schema = "CREATE TRIGGER IF NOT EXISTS sms_update_thread_on_delete AFTER DELETE ON sms_tb 	\
        BEGIN  \
        UPDATE threads_tb SET    date = OLD.date	\
        WHERE threads_tb.thread_id = OLD.thread_id;\
        UPDATE threads_tb SET message_count =\
        (SELECT COUNT(sms_tb.sms_id) FROM sms_tb  WHERE thread_id = OLD.thread_id AND sms_tb.type != 3)  WHERE threads_tb.thread_id = OLD.thread_id; \
        UPDATE threads_tb SET unread_count =  (SELECT COUNT(*)   FROM sms_tb  WHERE read = 0 AND type != 3  AND thread_id = OLD.thread_id) WHERE threads_tb.thread_id = OLD.thread_id;\
        DELETE FROM threads_tb WHERE thread_id=OLD.thread_id AND thread_id NOT IN (SELECT  thread_id FROM sms_tb) ;\
        END;";

#if 0
//insert a new thread id into canonical_addr_tb, we also need to add a new row into threads_tb
static const CHAR* s_threads_update_on_new_thread_trigger_schema = "CREATE TRIGGER IF NOT EXISTS new_thread_on_insert AFTER INSERT ON canonical_addr_tb 	\
        BEGIN  \
        INSERT INTO threads_tb (ID) VALUES (NEW.ID);\
        END;";
#endif

static const CHAR* s_sms_index_schema = "CREATE INDEX IF NOT EXISTS type_thread_index ON sms_tb (type, date DESC);";

static const CHAR* s_sms_thread_id_index_schema = "CREATE INDEX IF NOT EXISTS thread_index ON sms_tb (thread_id,date DESC);";

static const CHAR* s_thread_address_index_schema = "CREATE INDEX IF NOT EXISTS thread_address_index ON threads_tb (canonical_address);";

static const CHAR* s_thread_date_index_schema = "CREATE INDEX IF NOT EXISTS thread_date_index ON threads_tb (date );";

static INT32 _sms_create_table_impl(DatabaseHandle* db_handle,const char* tb_name,const CHAR* schema_str,BOOL force_new_table)
{


    if (!force_new_table && db_table_is_exist(db_handle,tb_name))
        return DB_OK;

    if (force_new_table)
    {
        db_table_drop(db_handle,tb_name);
    }

    return db_execSQL(db_handle,(const char*)schema_str);
}

INT32 sms_create_tables(DatabaseHandle* db_handle)
{
    return _sms_create_table_impl(db_handle,SMS_TABLE_NAME,s_sms_schema,FALSE);

}


INT32 sms_create_common_tables(DatabaseHandle* db_handle)
{
    INT32 result = 0;
    // result =  _sms_create_table_impl(db_handle,SMS_CANONICAL_ADDRESS_TABLE_NAME,s_canonical_addr_schema,FALSE);
    result =  _sms_create_table_impl(db_handle,SMS_THREADS_TABLE_NAME,s_threads_schema,FALSE);
    return result;

}
INT32 sms_create_triggers(DatabaseHandle* db_handle)
{
    INT32 result = 0;
    result =  db_execSQL(db_handle,s_threads_update_on_insert_trigger_schema);
    result =  db_execSQL(db_handle,s_threads_update_on_delete_trigger_schema);
    result =  db_execSQL(db_handle,s_threads_update_on_read_trigger_schema);
//    result =  db_execSQL(db_handle,s_threads_update_on_new_thread_trigger_schema);

    return result;

}
INT32 sms_create_index(DatabaseHandle* db_handle)
{
    INT32 result = 0;
    result =  _sms_create_table_impl(db_handle,"type_thread_index",s_sms_index_schema,FALSE);
    result =  _sms_create_table_impl(db_handle,"thread_index",s_sms_thread_id_index_schema,FALSE);
    result =  _sms_create_table_impl(db_handle,"thread_address_index",s_thread_address_index_schema,FALSE);
    result =  _sms_create_table_impl(db_handle,"thread_date_index",s_thread_date_index_schema,FALSE);
    return result;

}

INT32 sms_drop_all(DatabaseHandle* db_handle)
{
    INT32 result = 0;
    result =  db_execSQL(db_handle,"DROP TABLE IF EXISTS sms_tb;");
    result =  db_execSQL(db_handle,"DROP TABLE IF EXISTS threads_tb;");
    // result =  db_execSQL(db_handle,"DROP TABLE IF EXISTS canonical_addr_tb;");
    return result;

}
static INT32 sms_get_thread_id(DatabaseHandle* db_handle,const CHAR* address,INT64* thread_id_output)
{
    INT32 result = DB_ERROR;
    INT32 thread_id = 0;
    Cursor* cursor = NULL;
    DatabaseWhereClause* clause = NULL;
    return_val_if_fail(address&&thread_id_output, DB_ERROR);

    clause =  db_where_clause_create("canonical_address=?",1);
    db_clause_put_string(clause, 0, address);

    cursor = db_query(db_handle ,SMS_THREADS_TABLE_NAME,SMS_THREADS_TABLE_COL_ID,clause,NULL,NULL);

    db_clause_destroy(clause);

    if (!cursor)
        return DB_ERROR;

    if (cursor_get_count(cursor)>0)
    {
        result=cursor_get_int(cursor, 0,thread_id_output )?DB_OK:DB_ERROR;
    }

    cursor_destroy(cursor);
    return result;
}

#if 0 //sms is seperate with contact
static CHAR* _sms_query_name_from_contacts(const CHAR* address)
{
    DatabaseHandle* hdl = NULL;
    INT32 db_ret = db_open_v2(CONTACT_DB_PATH,DB_OPEN_READWRITE,&hdl);
    //CHAR* table_name = "contacts_me_tb me LEFT JOIN (SELECT me.ID,name FROM contacts_extension_tb_number ext INNER JOIN contacts_me_tb me ON me.ID=ext.ID ) tmp_tb ON tmp_tb.phonenumber=number";

    CHAR* table_name = "contacts_me_tb me INNER JOIN contacts_extension_tb_number ext ON me.ID=ext.ID";

    DatabaseWhereClause* clause = NULL;
    CHAR *statement=NULL;
    Cursor* cursor = NULL;
    CHAR* name  = NULL;
    INT32 len = 0;
    return_val_if_fail(hdl, NULL);
    len = strlen((const char*)address);
    statement = TG_CALLOC_V2(500);
    return_val_if_fail(statement, NULL);

    sprintf(statement, "ext.value GLOB '%s*'",address);
    clause = db_clause_create(DB_WHERE_CLAUSE, statement, 0);
    cursor = db_query(hdl,table_name,"name",clause,NULL,NULL);


    if (0 != cursor_get_count(cursor))
    {

        name = tg_strdup(cursor_get_string(cursor,0));

    }
    else
        name = tg_strdup(address);
    TG_FREE(statement);
    db_clause_destroy(clause);
    cursor_destroy(cursor);
    db_close(hdl);
    return name;
}
#else
static CHAR* _sms_query_name_from_contacts(DatabaseHandle* db_handle,const CHAR* address)
{
    CHAR* table_name = NULL;
    

    

    Cursor* cursor = NULL;
    CHAR* name  = NULL;
    INT32 len = 0;
    
    len = strlen((const char*)address);
    table_name = TG_CALLOC_V2(400);

    return_val_if_fail(table_name, NULL);

    sprintf(table_name, "(SELECT ext.ID FROM contacts_extension_tb_number ext WHERE ext.value='%s') tmp_tb LEFT JOIN contacts_me_tb me ON tmp_tb.ID=me.ID",address);

    cursor = db_query(db_handle,table_name,"name",NULL,NULL,NULL);

    
    if (0 != cursor_get_count(cursor))
    {

        name = tg_strdup(cursor_get_string(cursor,0));

    }
    else
        name = tg_strdup(address);
    TG_FREE(table_name);
    cursor_destroy(cursor);
    
    return name;
}
#if 0
static CHAR* _sms_query_name_from_contacts(const CHAR* address)
{
    return tg_strdup(address);
}
#endif

#endif
INT32 sms_update_thread(DatabaseHandle* db_handle, INT64 thread_id)
{
    INT32 result = DB_ERROR;
    DatabaseWhereClause* clause = NULL;
    CHAR* sql_str = NULL;

    if (thread_id < 0)
    {
        return DB_ERROR;//sms_update_all_threads(db_handle);
    }

    // Delete the row for this thread in the threads table and canonical table if
    // there are no more messages attached to it in either
    // the sms or pdu tables.
    clause = db_where_clause_create("ID = ? AND ID NOT IN (SELECT DISTINCT thread_id FROM sms_tb)",1);

    db_clause_put_int(clause,0,thread_id);

    result = db_delete(db_handle,SMS_THREADS_TABLE_NAME,clause);
    if (result  == DB_OK && db_last_changes(db_handle) > 0 )
    {

        db_clause_destroy(clause);
        // If this deleted the thread id, we have no more work to do.
        return DB_OK;
    }
    db_clause_destroy(clause);


    sql_str = TG_CALLOC_V2(200);
    return_val_if_fail(sql_str, DB_NOMEM);

    //UPDATE message count
    sprintf(sql_str,"UPDATE threads_tb SET message_count =(SELECT COUNT(sms_tb.ID) FROM sms_tb  WHERE thread_id = %ld AND sms_tb.type != 3)",thread_id);
    result = db_execSQL(db_handle, sql_str);

    //update date,maybe no need the sms type in where clause
    sprintf(sql_str,"UPDATE threads_tb SET date =(SELECT date FROM sms_tb  WHERE thread_id = %ld AND sms_tb.type != 3 ORDER BY date DESC LIMIT 1;)",thread_id);
    result = db_execSQL(db_handle, sql_str);

    //update unread count
    sprintf(sql_str,"UPDATE threads_tb SET unread_count =(SELECT COUNT(sms_tb.read) FROM sms_tb  WHERE thread_id = %ld AND read =0)",thread_id);
    result = db_execSQL(db_handle, sql_str);

    //update fail count  //todo,maybe no need this feature

    //update name
    //name = _sms_query_name_from_contacts();

    TG_FREE(sql_str);
    return result;
}

INT32 sms_update_cache_name_of_thread(DatabaseHandle* db_handle, INT64 thread_id,CHAR* address)
{
    INT32 result = DB_ERROR;
    DatabaseWhereClause* clause = NULL;
    CHAR* name = NULL;
    CHAR* sql_str = NULL;

    if (thread_id < 0)
    {
        return DB_ERROR;//sms_update_all_threads(db_handle);
    }

    //update name
    name = _sms_query_name_from_contacts(db_handle,address);
    sprintf(sql_str,"UPDATE threads_tb SET cache_name =%s WHERE thread_id = %ld;",name,thread_id);
    result = db_execSQL(db_handle, sql_str);

    TG_FREE(sql_str);
    return result;
}

INT32 sms_update_all_threads(DatabaseHandle* db_handle)
{
    INT32 result = DB_ERROR;
    Cursor* cursor = NULL;
    INT64 thread_id = -1;
    DatabaseWhereClause* clause = NULL;
    return_val_if_fail(cursor,DB_OK);
    clause= db_where_clause_create("ID NOT IN (SELECT DISTINCT thread_id FROM sms_tb)",0);
    result=db_delete(db_handle, SMS_THREADS_TABLE_NAME,clause);
    db_clause_destroy( clause);

    cursor = db_raw_query(db_handle, "SELECT thread_id FROM threads_tb");
    do
    {
        if (!cursor_get_int(cursor, 0, &thread_id))
            break;

        result=sms_update_thread(db_handle, thread_id);

    }while (cursor_move_to_next(cursor)&&result==DB_OK);

    cursor_destroy(cursor);


    return result;
}

INT32 sms_update_all_cache_name_in_threads(DatabaseHandle* db_handle)
{
    INT32 result = DB_ERROR;
    Cursor* cursor = NULL;
    INT64 thread_id = -1;
    CHAR* address = NULL;
    DatabaseWhereClause* clause = NULL;
    return_val_if_fail(cursor,DB_OK);
    clause= db_where_clause_create("ID NOT IN (SELECT DISTINCT thread_id FROM sms_tb)",0);
    result=db_delete(db_handle, SMS_THREADS_TABLE_NAME,clause);
    db_clause_destroy( clause);

    cursor = db_raw_query(db_handle, "SELECT thread_id,address FROM threads_tb");
    do
    {
        if (!cursor_get_int(cursor, 0, &thread_id))
            break;
        address= (CHAR*)cursor_get_string( cursor, 1);
        if (address)
            result=sms_update_cache_name_of_thread(db_handle, thread_id,address);

    }while (cursor_move_to_next(cursor)&&result==DB_OK);

    cursor_destroy(cursor);



    return result;
}


INT32 sms_get_or_create_thread_id(DatabaseHandle* db_handle,const CHAR* address,INT64* thread_id_output)
{
    INT32 result =DB_OK;
    ContentValues* values  = NULL;
    CHAR* name = NULL;
    return_val_if_fail(address&&thread_id_output, DB_ERROR);
    result = sms_get_thread_id(db_handle,address,thread_id_output);
    if (result == DB_OK)
        return result;
    values = content_values_create();
    return_val_if_fail(values, DB_NOMEM);

    content_values_put_string(values,SMS_THREADS_TABLE_COL_ADDRESS,address);
    //query the cached name from contacts db
    name = _sms_query_name_from_contacts(db_handle,address);
    if (name)
    {
        content_values_put_string(values,SMS_THREADS_TABLE_COL_CACHED_NAME,name);
        TG_FREE(name);
    }

    result= db_insert(db_handle,SMS_THREADS_TABLE_NAME,values);
    content_values_destroy(values);
    if (result==DB_OK)
        *thread_id_output = db_last_insert_rowid(db_handle);   //it is a skill, if the key id is auto increate id, it is the same with rowid

    return result;
}

#if 0
INT32 sms_delete_one(DatabaseHandle* db_handle, )
{
    INT64 thread_id = -1;
    Cursor* cursor =
        update
    }

#endif

