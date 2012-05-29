
#ifndef _CONTENT_PROVIDER_INTERNAL_H_
#define _CONTENT_PROVIDER_INTERNAL_H_

#include "content_provider.h"
#include <tg_utility.h>
#include <db_wrapper.h>
#include <db_clause.h>

typedef INT32 (*CP_CREATE_INTERNAL)();

typedef INT32 (*CP_OPEN_INTERNAL)();

typedef INT32 (*CP_CLOSE_INTERNAL)(ContentProvider* provider);

typedef const CHAR* (*CP_GET_ID)();


typedef INT32 (*CP_INSERT_INTERNAL)(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num,ContentValues* values);

typedef INT32 (*CP_UPDATE_INTERNAL)(ContentProvider* provider,ContentProviderDestination dest, ContentValues* values,DatabaseWhereClause* filter);

typedef INT32 (*CP_DELETE_INTERNAL)(ContentProvider* provider,ContentProviderDestination dest, DatabaseWhereClause* filter);

typedef Cursor* (*CP_QUERY_INTERNAL)(ContentProvider* provider,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where_clause,const CHAR* order_by,const CHAR* limit);

typedef Cursor* (*CP_QUERY_V2_INTERNAL)(ContentProvider* provider,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where_clause,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit);

typedef INT32 (*CP_GET_COUNT_INTERNAL)(ContentProvider* thiz,ContentProviderDestination dest);

typedef INT32 (*CP_GET_COUNT_V2_INTERNAL)(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause);

typedef INT32 (*CP_GET_LAST_ERROR_INTERNAL)(ContentProvider* thiz);

typedef DatabaseHandle* (*CP_GET_DB_HANDLE_INTERNAL)(ContentProvider* thiz);

typedef INT32 (*CP_RELEASE_DB_HANDLE_INTERNAL)(ContentProvider* thiz);


typedef struct  /* PROVIDER PROCEDURE POINTERS */
{		
    CP_GET_ID			get_id;
    CP_OPEN_INTERNAL  create;
    CP_OPEN_INTERNAL  open;
    CP_OPEN_INTERNAL close;

    CP_INSERT_INTERNAL insert;
    CP_QUERY_INTERNAL   query;
    CP_QUERY_V2_INTERNAL query_v2;
    CP_UPDATE_INTERNAL update;
    CP_DELETE_INTERNAL delete;

    CP_GET_COUNT_INTERNAL get_count;
    CP_GET_COUNT_V2_INTERNAL get_count_v2;

   CP_GET_LAST_ERROR_INTERNAL get_last_error;

   CP_GET_DB_HANDLE_INTERNAL get_db_handle;
   CP_RELEASE_DB_HANDLE_INTERNAL release_db_handle;
}ContentProviderProcess;


struct  _ContentProvider/* PROVIDER instance */
{		  
    BOOL read_only;
    ContentProviderProcess* proc;
    void* priv_data;
   
};


//aync 

typedef enum
{
    CONTENT_PROVIDER_QUERY_MSG_ID,
}CONTENT_PROVIDER_MSG_ID;
typedef struct
{
    CONTENT_PROVIDER_MSG_ID msg_id;
    ContentProvider*  provider;
    ContentProviderDestination  dest;
    void* 				msg_para;
    CONTENT_PROVIDER_CB  cb;
    void*					 user_data;
}CONTENT_PROVIDER_MSG;

//query message parameter
typedef struct
{
    
    CHAR* 				col_list;
    DatabaseWhereClause* 	where;
    CHAR*				 group_by;
    DatabaseHavingClause*       having;
    CHAR*	                      order_by;
    CHAR*	                      limit;
    
	
}CONTENT_PROVIDER_QUERY_MSG;


#endif




