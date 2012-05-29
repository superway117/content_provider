#include "CUnit.h"
#include "Automated.h"

#include "../content_provider.h"
#include <common.h>
#include <tg_utility.h>
#include "../calllog_provider.h"

static int calllog_provider_test_suit_init(void)
{

    return 0;
}
static int calllog_provider_test_suit_clean(void)
{
    tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void calllog_provider_insert_test()  
{	
	INT32 ret = 0;
	INT32 i = 0;
	CHAR number[20];

	ContentProvider* cp = content_provider_open(CALLLOG_PROVIDER_ID, FALSE);
	ContentValues* values = NULL;
	return_if_fail(cp);
	content_provider_delete(cp,CALLLOG_PROVIDER_DESTINATION_LIST,NULL);
	values = content_values_create();
	if(!values)
	{
		CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
		return;
	}
	for(i=0;i<1000;i++)
	{
		
		sprintf(number,"029883282%d",i);
		content_values_put_string(values,"number",number);

		content_values_put_int(values,"type",0);

		content_values_put_int(values,"duration",10);

		content_values_put_int(values,"time",20110309);
		
		ret = content_provider_insert(cp,CALLLOG_PROVIDER_DESTINATION_LIST,"number,type,duration,time",4,values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	content_values_destroy(values);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

	
}

static void calllog_provider_query_test()
{	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(CALLLOG_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, CALLLOG_PROVIDER_DESTINATION_LIST, "name,sim,number,type,duration,time",NULL,"time",NULL);
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = NULL;
		//CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"name");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"sim");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"number");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"type");
		key_name = cursor_get_column_name(cursor,4);
		CU_ASSERT_STRING_EQUAL(key_name,"duration");
		key_name = cursor_get_column_name(cursor,5);
		CU_ASSERT_STRING_EQUAL(key_name,"time");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 type;
				INT64 duration;
				INT64 time;
				INT64 sim;
				CHAR* name =NULL;
				CHAR* number =NULL;
				
				name=cursor_get_string(cursor,0);
				if(name)
					db_log("query name=%s\n",name);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,1,&sim),TRUE);
				number= cursor_get_string(cursor,2);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,3,&type),TRUE);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,4,&duration),TRUE);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,5,&time),TRUE);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

	cursor_destroy(cursor);
}

static void calllog_provider_query_v2_test()
{	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(CALLLOG_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query_v2(cp, CALLLOG_PROVIDER_DESTINATION_LIST, "name,sim,number,type,duration,time",NULL,NULL,NULL,"time","0,100");
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = NULL;
		key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"name");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"sim");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"number");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"type");
		key_name = cursor_get_column_name(cursor,4);
		CU_ASSERT_STRING_EQUAL(key_name,"duration");
		key_name = cursor_get_column_name(cursor,5);
		CU_ASSERT_STRING_EQUAL(key_name,"time");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 type;
				INT64 duration;
				INT64 time;
				INT64 sim;
				CHAR* name =NULL;
				CHAR* number =NULL;
				
				name=cursor_get_string(cursor,0);
				if(name)
					db_log("query name=%s\n",name);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,1,&sim),TRUE);
				number= cursor_get_string(cursor,2);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,3,&type),TRUE);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,4,&duration),TRUE);
				CU_ASSERT_EQUAL(cursor_get_int(cursor,5,&time),TRUE);
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

	cursor_destroy(cursor);
}
static void calllog_provider_update_test()
{
}

static void calllog_provider_delete_test()
{	

	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(CALLLOG_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	ret=content_provider_delete(cp, CALLLOG_PROVIDER_DESTINATION_LIST, NULL);
	CU_ASSERT_EQUAL(ret,DB_OK);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}
static void calllog_provider_get_count_test()
{	

	INT32 count = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(CALLLOG_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	count=content_provider_get_count(cp, CALLLOG_PROVIDER_DESTINATION_LIST);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}

static CU_TestInfo calllog_provider_testcases[] =
{ 

   {"calllog_provider_insert_test",calllog_provider_insert_test},
   {"calllog_provider_query_test",calllog_provider_query_test},  //update
   	{"calllog_provider_query_v2_test",calllog_provider_query_v2_test},
    {"calllog_provider_get_count_test:", calllog_provider_get_count_test},  //delete
    {"calllog_provider_delete_test:", calllog_provider_delete_test},  //delete
    

    CU_TEST_INFO_NULL
};


static CU_SuiteInfo calllog_provider_suites[] = {
    {"Testing calllog_provider_testcases:", NULL, calllog_provider_test_suit_clean, calllog_provider_testcases},
    CU_SUITE_INFO_NULL
};


static void calllog_provider_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(calllog_provider_suites))
    {
        return;
    }
}

void calllog_provider_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        calllog_provider_add_tests();
        CU_set_output_filename("calllog_provider_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}

