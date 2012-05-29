

#include "singleton_provider.h"
#include <uta_os.h>
#include <tg_utility.h>
//maybe the name of SingletonProvider is not very  appropriate, i designed it as a  component of a really provider, like contact provider and calllog provider, that do not inherit from SingletonProvider
#if 0
struct _SingletonProvider
{
    DatabaseHandle* db_handle;
    UtaOsSem*		  sem;
    INT32 open_count;
    BOOL in_use;
};
#endif

INT32 singleton_provider_create(SingletonProvider* thiz)
{
    if (thiz->sem==NULL)
    {
        thiz->sem = TG_CALLOC_V2(sizeof(UtaOsSem));
        UtaOsSemCreate(thiz->sem, "provider",1, UTA_OS_SEM_TIMEOUT_SUPPORT);
        thiz->open_count = 0;
        thiz->in_use= FALSE;
        thiz->db_handle=NULL;
    }
    return DB_OK;
}

INT32 singleton_provider_open(SingletonProvider* thiz,const CHAR* path)
{
    INT32 result = DB_OK;
    if (UTA_SUCCESS!=UtaOsSemObtain(thiz->sem, 1000, 1000))
        return DB_ERROR;
    if (thiz->db_handle==NULL)
    {
        result=db_open_v2(path, DB_OPEN_READWRITE,&thiz->db_handle);
        thiz->open_count = 1;
    }
    else
        thiz->open_count++;
    UtaOsSemRelease(thiz->sem);
    return result;
}
INT32 singleton_provider_close(SingletonProvider* thiz)
{
    INT32 result = DB_OK;
    if (UTA_SUCCESS!=UtaOsSemObtain(thiz->sem, 1000, 1000))
        return DB_ERROR;
    if (thiz->open_count>0)
        thiz->open_count--;
    if (thiz->open_count==0)
    {
        result=db_close(thiz->db_handle);
        thiz->db_handle= NULL;
    }
    UtaOsSemRelease(thiz->sem);
    return result;
}

DatabaseHandle* singleton_provider_obtain_db_handle(SingletonProvider* thiz)
{
    DatabaseHandle* hdl=NULL;
    if (UTA_SUCCESS!=UtaOsSemObtain(thiz->sem, 1000, 1000))
        return NULL;
    if (!thiz->in_use)
    {
        hdl = thiz->db_handle;
        thiz->in_use=TRUE;

    }
    UtaOsSemRelease(thiz->sem);
    return hdl;
}

INT32 singleton_provider_release_db_handle(SingletonProvider* thiz)
{

    if (UTA_SUCCESS!=UtaOsSemObtain(thiz->sem, 1000, 10000))
        return DB_ERROR;
    thiz->in_use=FALSE;
    UtaOsSemRelease(thiz->sem);
    return DB_OK;
}

INT32 singleton_provider_get_last_error(SingletonProvider* thiz)
{
    return_val_if_fail(thiz&&thiz->db_handle, DB_ERROR);
    return db_last_error(thiz->db_handle);
}
