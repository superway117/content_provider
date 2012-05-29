
#ifndef _SINGLETON_PROVIDER_H_
#define _SINGLETON_PROVIDER_H_
                                                   
#include <common.h>
#include <db_wrapper.h>
#include <uta_os.h>
//struct _SingletonProvider;

//maybe the name of SingletonProvider is not very  appropriate, i designed it as a  component of a really provider, like contact provider and calllog provider, that do not inherit from SingletonProvider
//typedef struct _SingletonProvider SingletonProvider;
typedef struct _SingletonProvider
{
    DatabaseHandle* db_handle;
    UtaOsSem*		  sem;
    INT32 open_count;
    BOOL in_use;
}SingletonProvider;


extern INT32 singleton_provider_create(SingletonProvider* thiz);

extern INT32 singleton_provider_open(SingletonProvider* thiz,const CHAR* path);

extern INT32 singleton_provider_close(SingletonProvider* thiz);

extern DatabaseHandle* singleton_provider_obtain_db_handle(SingletonProvider* thiz);

extern INT32 singleton_provider_release_db_handle(SingletonProvider* thiz);
extern INT32 singleton_provider_get_last_error(SingletonProvider* thiz);
#endif
