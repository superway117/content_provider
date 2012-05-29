
#include "singleton_provider.h"


typedef struct 
{
    SingletonProvider singleton_provider;
}ContactProviderBase;

#define CONTACT_DB_PATH "/nvram/contacts.db"



static ContactProviderBase s_contact_base_provider={0};

INT32 contact_base_provider_create()
{
    return singleton_provider_create(&s_contact_base_provider.singleton_provider);

}

INT32 contact_base_provider_open()
{
    return singleton_provider_open(&s_contact_base_provider.singleton_provider,CONTACT_DB_PATH);
}
INT32 contact_base_provider_close()
{
    return singleton_provider_close(&s_contact_base_provider.singleton_provider);
}

DatabaseHandle* contact_base_provider_obtain_db_handle()
{
    return singleton_provider_obtain_db_handle(&s_contact_base_provider.singleton_provider);
}

INT32 contact_base_provider_release_db_handle()
{
    return singleton_provider_release_db_handle(&s_contact_base_provider.singleton_provider);
}
INT32 contact_base_provider_get_last_error()
{
    return singleton_provider_get_last_error(&s_contact_base_provider.singleton_provider);
}
