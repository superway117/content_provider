#include "CUnit.h"
#include "Automated.h"

#include "../content_provider.h"
#include "../sms_provider.h"
#include <common.h>
#include <tg_utility.h>

static int sms_provider_test_suit_init(void)
{

    return 0;
}
static int sms_provider_test_suit_clean(void)
{
  //  tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void sms_provider_insert_test()  
{
	
	INT32 ret = 0;
	INT32 i = 0;
	CHAR address[30];
	CHAR body[100];
//	CHAR date[50];
	ContentProvider* cp = content_provider_open(SMS_PROVIDER_ID, FALSE);
	
	return_if_fail(cp);
	ret=content_provider_delete(cp,SMS_PROVIDER_DESTINATION_LIST,NULL);
	
	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_unread_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
	//	sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_INBOX);

		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,date,type",4,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_read_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
		//sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());

		
		content_values_put_int(values,SMS_PROVIDER_COL_READ,1);
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_INBOX);
		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,read,date,type",5,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}

	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_read_sentbox_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
		//sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());

		
		content_values_put_int(values,SMS_PROVIDER_COL_READ,1);
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_SENT);
		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,read,date,type",5,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_read_draftbox_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
		//sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());

		
		content_values_put_int(values,SMS_PROVIDER_COL_READ,1);
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_OUTBOX);
		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,read,date,type",5,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_read_outbox_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
		//sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());

		
		content_values_put_int(values,SMS_PROVIDER_COL_READ,1);
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_DRAFT);
		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,read,date,type",5,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}

	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_read_failed_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
		//sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());

		
		content_values_put_int(values,SMS_PROVIDER_COL_READ,1);
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_FAILED);
		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,read,date,,type",5,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}

	for(i=0;i<30;i++)
	{

		ContentValues* values = NULL;
		values = content_values_create();
		sprintf(address,"029883282%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_ADDRESS,address);
		sprintf(body,"BODY_text_sms_inbox_read_queued_%d",i);
		content_values_put_string(values,SMS_PROVIDER_COL_BODY,body);
		//sprintf(date,"201107%d",i);
		content_values_put_int(values,SMS_PROVIDER_COL_DATE,(INT64)util_GetCurrentAbsoluteSeconds());

		
		content_values_put_int(values,SMS_PROVIDER_COL_READ,1);
		content_values_put_int(values,SMS_PROVIDER_COL_TYPE,SMS_MESSAGE_TYPE_QUEUED);
		ret = content_provider_insert(cp,SMS_PROVIDER_DESTINATION_LIST,"address,body,read,date,type",5,values);
		content_values_destroy(values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

	
}

static void sms_provider_query_test()
{
	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(SMS_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, SMS_PROVIDER_DESTINATION_CONVERSATION, "thread_id,cached_name,canonical_address,date,message_count,unread_count",NULL,"date","0,20");
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"thread_id");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"cached_name");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"canonical_address");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"date");
		key_name = cursor_get_column_name(cursor,4);
		CU_ASSERT_STRING_EQUAL(key_name,"message_count");
		key_name = cursor_get_column_name(cursor,5);
		CU_ASSERT_STRING_EQUAL(key_name,"unread_count");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 datetime;
				INT64 msg_count;
				INT64 unread_count;
				CHAR* name =NULL;
				CHAR* address =NULL;
				CHAR* time =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);

					
				name= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( name);
	
				address= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( address);

				CU_ASSERT_EQUAL(cursor_get_int(cursor,3,&datetime),TRUE);


				CU_ASSERT_EQUAL(cursor_get_int(cursor,4,&msg_count),TRUE);

				
				CU_ASSERT_EQUAL(cursor_get_int(cursor,5,&unread_count),TRUE);

				
				db_log("ID:%ld; cached_name:%s; Addr : %s; Date:%ld ; Count:%ld; Unread:%ld",(INT32)id,name,address,(INT32)datetime,(INT32)msg_count,(INT32)unread_count);

				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}
static void sms_provider_query_v2_test()
{
	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(SMS_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, SMS_PROVIDER_DESTINATION_CONVERSATION, "thread_id,name,canonical_address,date,message_count,unread_count",NULL,"date","0,50");
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"thread_id");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"name");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"canonical_address");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"date");
		key_name = cursor_get_column_name(cursor,4);
		CU_ASSERT_STRING_EQUAL(key_name,"message_count");
		key_name = cursor_get_column_name(cursor,5);
		CU_ASSERT_STRING_EQUAL(key_name,"unread_count");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 datetime;
				INT64 msg_count;
				INT64 unread_count;
				CHAR* name =NULL;
				CHAR* address =NULL;
				CHAR* time =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);

					
				name= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( name);
	
				address= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( address);

				CU_ASSERT_EQUAL(cursor_get_int(cursor,3,&datetime),TRUE);


				CU_ASSERT_EQUAL(cursor_get_int(cursor,4,&msg_count),TRUE);

				
				CU_ASSERT_EQUAL(cursor_get_int(cursor,5,&unread_count),TRUE);

				
				db_log("ID:%ld; Name:%s; Addr : %s; Date:%ld ; Count:%ld; Unread:%ld",(INT32)id,name,address,(INT32)datetime,(INT32)msg_count,(INT32)unread_count);

				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}

static void sms_provider_query_sms_test()
{
	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(SMS_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, SMS_PROVIDER_DESTINATION_LIST, "sms_id,name,address,date",NULL,"date","10,20");
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
	
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 datetime;
				CHAR* name =NULL;
				CHAR* address =NULL;
				CHAR* time =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);

					
				name= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( name);
	
				address= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( address);

				CU_ASSERT_EQUAL(cursor_get_int(cursor,3,&datetime),TRUE);


				db_log("ID:%ld; Name:%s; Addr : %s; Date:%ld;",(INT32)id,name,address,(INT32)datetime);
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}
static void sms_provider_query_async_test()
{
}
static void sms_provider_update_test()
{
}

static void sms_provider_delete_test()
{
	INT32 ret; 
	ContentProvider* cp = content_provider_open(SMS_PROVIDER_ID, FALSE);
	ret=content_provider_delete(cp,SMS_PROVIDER_DESTINATION_LIST,NULL);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
}

static void sms_provider_get_count_test()
{

	INT32 count = 0;
	INT32 i = 0;

	ContentProvider* cp = content_provider_open(SMS_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	count=content_provider_get_count_v2(cp,SMS_PROVIDER_DESTINATION_LIST,"type=0");
	db_log("sms_provider_get_count_test inbox count : %d",count);
	count=content_provider_get_count_v2(cp,SMS_PROVIDER_DESTINATION_LIST,"type=1");
	db_log("sms_provider_get_count_test sent count : %d",count);
	count=content_provider_get_count_v2(cp,SMS_PROVIDER_DESTINATION_LIST,"type=2");
	db_log("sms_provider_get_count_test outbox count : %d",count);
	count=content_provider_get_count_v2(cp,SMS_PROVIDER_DESTINATION_LIST,"type=3");
	db_log("sms_provider_get_count_test draft count : %d",count);
	count=content_provider_get_count_v2(cp,SMS_PROVIDER_DESTINATION_LIST,"type=4");
	db_log("sms_provider_get_count_test failed count : %d",count);
	count=content_provider_get_count_v2(cp,SMS_PROVIDER_DESTINATION_LIST,"type=5");
	db_log("sms_provider_get_count_test queue count : %d",count);
	count=content_provider_get_count(cp,SMS_PROVIDER_DESTINATION_LIST);
	db_log("sms_provider_get_count_test all count : %d",count);

	count=content_provider_get_count(cp,SMS_PROVIDER_DESTINATION_CONVERSATION);
	db_log("sms_provider_get_count_test group count : %d",count);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}
static CU_TestInfo sms_provider_testcases[] =
{ 
    {"sms_provider_insert_test",sms_provider_insert_test},
    {"sms_provider_query_test",sms_provider_query_test},  //update
    {"sms_provider_query_v2_test",sms_provider_query_v2_test},
    {"sms_provider_query_sms_test",sms_provider_query_sms_test},
//    {"sms_provider_update_test:", sms_provider_update_test},  //delete
    {"sms_provider_get_count_test:", sms_provider_get_count_test},  //delete
//    {"sms_provider_insert_test",sms_provider_insert_test},
	{"sms_provider_query_async_test:", sms_provider_query_async_test},  //delete
//	{"sms_provider_delete_test",sms_provider_delete_test},
    CU_TEST_INFO_NULL
};


static CU_SuiteInfo sms_provider_suites[] = {
    {"Testing sms_provider_testcases:", NULL, sms_provider_test_suit_clean, sms_provider_testcases},
    CU_SUITE_INFO_NULL
};


static void sms_provider_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(sms_provider_suites))
    {
        return;
    }
}

void sms_provider_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        sms_provider_add_tests();
        CU_set_output_filename("sms_provider_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}

