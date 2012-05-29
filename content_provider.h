
#ifndef _CONTENT_PROVIDER_H_
#define _CONTENT_PROVIDER_H_
#include <common.h>
#include <db_wrapper.h>

typedef struct _ContentProvider ContentProvider;

typedef INT32 ContentProviderDestination;

//aync  callback
typedef INT32 (*CONTENT_PROVIDER_CB)(ContentProvider* thiz,INT32 error,void* user_data,void* para);


extern INT32  content_provider_init_all();

extern ContentProvider* content_provider_open(const CHAR* ID,BOOL read_only);
extern INT32 content_provider_close(ContentProvider* thiz);

extern INT32 content_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest);

extern INT32 content_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause);

extern INT32 content_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num,ContentValues* values);

extern Cursor* content_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit);

extern Cursor* content_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where_clause,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit);

extern INT32 content_provider_query_v2_asyn(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit,CONTENT_PROVIDER_CB cb,void* user_data);

extern INT32 content_provider_query_asyn(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where,const CHAR* order_by,const CHAR* limit,CONTENT_PROVIDER_CB cb,void* user_data);
extern INT32 content_provider_update(ContentProvider* thiz,ContentProviderDestination dest,ContentValues* values,DatabaseWhereClause* filter);

extern INT32 content_provider_delete(ContentProvider* thiz,ContentProviderDestination dest,DatabaseWhereClause* filter);

extern INT32 content_provider_get_last_error(ContentProvider* thiz);

extern INT32 content_provider_end_transation(ContentProvider* thiz);

extern INT32 content_provider_begin_transation(ContentProvider* thiz);


#endif




