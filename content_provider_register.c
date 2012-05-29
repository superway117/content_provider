
#include "content_provider_internal.h"

extern ContentProviderProcess g_contact_procs;

extern ContentProviderProcess g_calllog_procs;

extern ContentProviderProcess g_memo_procs;

extern ContentProviderProcess g_sms_procs;

static ContentProviderProcess *s_cp_procs_list[] = 
{
     &g_contact_procs,
    &g_calllog_procs,
    &g_memo_procs,
    &g_sms_procs,
};

#define NUM_OF_CP_LIST (sizeof s_cp_procs_list / sizeof s_cp_procs_list[0])


ContentProviderProcess *content_provider_find_proc(const CHAR* ID)
{
    INT32 i;
    INT32 register_num = NUM_OF_CP_LIST;
    return_val_if_fail(ID&&register_num>0,NULL);
    for(i=0;i<register_num;i++)
    {
    	ContentProviderProcess* procs = s_cp_procs_list[i];
    	if(0==strcmp(ID,procs->get_id()))
		return s_cp_procs_list[i];
    }

  
    return NULL;
}


INT32 content_provider_get_registered_num()
{
    return NUM_OF_CP_LIST;
}

ContentProviderProcess * content_provider_get_proc(INT32 idx)
{
    INT32 register_num = NUM_OF_CP_LIST;
    return_val_if_fail(idx<register_num,NULL);
    return s_cp_procs_list[idx];
   
}