
#ifndef _CONTACT_PROVIDER_BASE_H_
#define _CONTACT_PROVIDER_BASE_H_

#include "singleton_provider.h"

#define CONTACT_DB_PATH "/nvram/contacts.db"

//#define CONTACT_ME_TABLE_NAME "contacts_me_tb"

extern INT32 contact_base_provider_create(void);

extern INT32 contact_base_provider_open(void);

extern INT32 contact_base_provider_close(void);

extern DatabaseHandle* contact_base_provider_obtain_db_handle(void);

extern INT32 contact_base_provider_release_db_handle(void);

extern INT32 contact_base_provider_get_last_error(void);
#endif

