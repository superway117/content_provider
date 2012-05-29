
#include "content_provider_internal.h"
#include <uta_os.h>

extern ContentProviderProcess *content_provider_find_proc(const CHAR* ID);

extern  ContentProviderProcess * content_provider_get_proc(INT32 idx);

extern INT32 content_provider_get_registered_num();

static UtaOsThread s_provider_thread;

static UtaOsQueue                                       s_provider_queue;
static UINT32                                           s_provider_queue_buf[256];


#define PROVIDER_THREAD_STACK_SIZE_IMGLIB  1024
#ifndef WIN32
__align(8) static char       s_provider_stack_buf[PROVIDER_THREAD_STACK_SIZE_IMGLIB];
#else
static char                           s_provider_stack_buf[PROVIDER_THREAD_STACK_SIZE_IMGLIB];
#endif

static void content_provider_destroy_query_msg(CONTENT_PROVIDER_QUERY_MSG* query_msg);

static void content_provider_thread_main(CONTENT_PROVIDER_MSG* msg)
{

    ContentProviderDestination  dest;
    ContentProvider*  provider;
    return_if_fail(msg);
    provider=msg->provider;
    dest=msg->dest;
    if (!provider)
    {
        TG_FREE(msg);
        return;
    }
    if (msg->msg_id==CONTENT_PROVIDER_QUERY_MSG_ID)
    {
        CONTENT_PROVIDER_QUERY_MSG* query_msg = (CONTENT_PROVIDER_QUERY_MSG*)msg->msg_para;
        Cursor* cursor = NULL;
        if (query_msg && msg->cb)
        {
            cursor=provider->proc->query_v2(provider,dest,query_msg->col_list,query_msg->where,query_msg->group_by,query_msg->having,query_msg->order_by,query_msg->limit);
            content_provider_destroy_query_msg(query_msg);
            if (msg->cb)
                msg->cb(provider,content_provider_get_last_error(provider),msg->user_data,(void*)cursor);
        }
    }
    TG_FREE(msg);
}


static void content_provider_thread_entry(void* p)
{
    UINT32 msg;
    UtaInt status;

    while (1)
    {
        status = UtaOsQueueReceive(&s_provider_queue, &msg, UTA_TRUE);

        if (status == UTA_SUCCESS)
            content_provider_thread_main((CONTENT_PROVIDER_MSG*)msg);

    }
}


static BOOL content_provider_create_thread()
{
    UtaInt status;

    status = UtaOsQueueCreate(&s_provider_queue, "provider",s_provider_queue_buf,4, sizeof(s_provider_queue_buf) / 4);

    /* Create Task */
    status = UtaOsThreadCreate(&s_provider_thread, "provider",(UtaOsThreadPriority)UTA_OS_THREAD_PRIO_APP_BACKGROUND_10, (void*)s_provider_stack_buf,
                               PROVIDER_THREAD_STACK_SIZE_IMGLIB,
                               (UtaOsThreadFn)content_provider_thread_entry,
                               0);
    status = UtaOsThreadStart(&s_provider_thread);
    SetThreadPriority();
    return (status == UTA_SUCCESS);
}

static BOOL content_provider_post_message(CONTENT_PROVIDER_MSG** msg)
{
    UtaInt status;

    return_val_if_fail(msg, FALSE);

    status = UtaOsQueueSend(&s_provider_queue, ( void *)(msg), UTA_FALSE);

    return (status == UTA_SUCCESS);
}

INT32  content_provider_init_all()
{
    INT32 num = content_provider_get_registered_num();
    ContentProviderProcess* procs=NULL;
    INT32 i=0;
    INT32 ret=DB_OK;
    content_provider_create_thread();
    return_val_if_fail(num>0,DB_ERROR);
    for (i=0;i<num;i++)
    {
        ContentProviderProcess* procs=content_provider_get_proc(i);
        if (procs->create)
            ret=procs->create();
    }
    return ret;
}




ContentProvider* content_provider_open(const CHAR* ID,BOOL read_only)
{
    ContentProviderProcess* procs=content_provider_find_proc(ID);
    ContentProvider* cp = NULL;
    return_val_if_fail(procs,NULL);
    cp = TG_CALLOC_V2(sizeof(ContentProvider));
    return_val_if_fail(cp,NULL);

    cp->read_only=read_only;
    cp->proc=procs;

    if (DB_OK==procs->open(cp))
        return cp;
    else
    {
        TG_FREE(cp);
        return NULL;
    }
}


INT32 content_provider_close(ContentProvider* thiz)
{
    INT32 ret=DB_OK;
    return_val_if_fail(thiz,DB_ERROR);
    ret=thiz->proc->close(thiz);
    if (ret==DB_OK)
        TG_FREE(thiz);
    return ret;
}

INT32 content_provider_get_count(ContentProvider* thiz,ContentProviderDestination dest)
{
    return_val_if_fail(thiz ,DB_ERROR);

    return thiz->proc->get_count(thiz,dest);
}

INT32 content_provider_get_count_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* where_clause)
{
    return_val_if_fail(thiz ,DB_ERROR);
    return thiz->proc->get_count_v2(thiz,dest,where_clause);
}

INT32 content_provider_insert(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list,INT32 col_num,ContentValues* values)
{
    return_val_if_fail(thiz ,DB_ERROR);
    return_val_if_fail(!thiz->read_only,DB_READONLY);
    return_val_if_fail(values,DB_ERROR);
    return thiz->proc->insert(thiz,dest,col_list,col_num,values);
}

static void content_provider_destroy_query_msg(CONTENT_PROVIDER_QUERY_MSG* query_msg)
{
    TG_FREE(query_msg->col_list );
    TG_FREE(query_msg->group_by );
    TG_FREE(query_msg->limit );
    TG_FREE(query_msg->order_by );
    db_clause_destroy(query_msg->where );
    db_clause_destroy(query_msg->having );
    TG_FREE(query_msg);
}
static CONTENT_PROVIDER_MSG* content_provider_build_query_msg(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where_clause,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit,CONTENT_PROVIDER_CB cb,void* user_data)
{
    BOOL ret=TRUE;
    CONTENT_PROVIDER_MSG* msg = NULL;//TG_CALLOC_V2(sizeof(CONTENT_PROVIDER_MSG));
    CONTENT_PROVIDER_QUERY_MSG* query_msg = TG_CALLOC_V2(sizeof(CONTENT_PROVIDER_QUERY_MSG));
    return_val_if_fail(query_msg, NULL);

    query_msg->col_list = tg_strdup(col_list);
    query_msg->group_by = tg_strdup(group_by);
    query_msg->limit = tg_strdup(limit);
    query_msg->order_by= tg_strdup(order_by);
    query_msg->where=db_clause_duplicate(where_clause);
    query_msg->having=db_clause_duplicate(having);
    if (where_clause )
    {
        if (!query_msg->where)
        {
            ret= FALSE;

        }

    }
    if (having )
    {
        if (!query_msg->having)
        {
            ret= FALSE;

        }

    }
    if (!ret)
    {
        content_provider_destroy_query_msg(query_msg);
        return NULL;
    }
    msg = TG_CALLOC_V2(sizeof(CONTENT_PROVIDER_MSG));

    msg->provider=thiz;
    msg->msg_id = CONTENT_PROVIDER_QUERY_MSG_ID;
    msg->dest = dest;
    msg->msg_para = (void*)query_msg;
    msg->cb = cb;
    msg->user_data = user_data;
    return msg;
}

Cursor* content_provider_query(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* clause,const CHAR* order_by,const CHAR* limit)
{
    return_val_if_fail(thiz ,NULL);
    return thiz->proc->query(thiz,dest,col_list,clause,order_by,limit);
}



Cursor* content_provider_query_v2(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where_clause,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit)
{
    return_val_if_fail(thiz,NULL);
    return thiz->proc->query_v2(thiz,dest,col_list,where_clause,group_by,having,order_by,limit);
}


INT32 content_provider_query_v2_asyn(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where,const CHAR* group_by,DatabaseHavingClause* having,const CHAR* order_by,const CHAR* limit,CONTENT_PROVIDER_CB cb,void* user_data)
{
    CONTENT_PROVIDER_MSG* msg = NULL;
    return_val_if_fail(thiz ,DB_ERROR);
    msg=content_provider_build_query_msg(thiz,dest,col_list,  where,group_by,having,order_by,limit,cb,user_data);
    return_val_if_fail(msg, DB_ERROR);
    if ( !content_provider_post_message(&msg))
    {
        content_provider_destroy_query_msg((CONTENT_PROVIDER_QUERY_MSG*)msg->msg_para);
        TG_FREE(msg);
        return DB_ERROR;
    }
    return DB_TODO;

}
INT32 content_provider_query_asyn(ContentProvider* thiz,ContentProviderDestination dest,const CHAR* col_list, DatabaseWhereClause* where,const CHAR* order_by,const CHAR* limit,CONTENT_PROVIDER_CB cb,void* user_data)
{
    return content_provider_query_v2_asyn(thiz,dest,col_list,where,NULL,NULL,order_by,limit,cb,user_data);

}
INT32 content_provider_update(ContentProvider* thiz,ContentProviderDestination dest, ContentValues* values, DatabaseWhereClause* filter)
{
    return_val_if_fail(thiz ,DB_ERROR);
    return_val_if_fail(values,DB_ERROR);
    return thiz->proc->update(thiz,dest,values,filter);
}

INT32 content_provider_delete(ContentProvider* thiz,ContentProviderDestination dest, DatabaseWhereClause* filter)
{
    return_val_if_fail(thiz,DB_ERROR);
    return_val_if_fail(!thiz->read_only,DB_READONLY);
    return thiz->proc->delete(thiz,dest,filter);
}

INT32 content_provider_get_last_error(ContentProvider* thiz)
{
    return_val_if_fail(thiz,DB_ERROR);
    return thiz->proc->get_last_error(thiz);
}

INT32 content_provider_begin_transation(ContentProvider* thiz)
{
    DatabaseHandle* hdl = NULL;
    INT32 ret = DB_OK;
    return_val_if_fail(thiz,DB_ERROR);
    hdl= thiz->proc->get_db_handle(thiz);
    return_val_if_fail(hdl,DB_ERROR);
    ret =  db_begin_transation(hdl);
    thiz->proc->release_db_handle(thiz);
    return ret;
}
INT32 content_provider_end_transation(ContentProvider* thiz)
{
    DatabaseHandle* hdl = NULL;
	INT32 ret = DB_OK;
    return_val_if_fail(thiz,DB_ERROR);
    hdl= thiz->proc->get_db_handle(thiz);
    return_val_if_fail(hdl,DB_ERROR);
    ret =  db_end_transation(hdl);
    thiz->proc->release_db_handle(thiz);
    return ret;
}

