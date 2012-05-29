#include "CUnit.h"
#include "Automated.h"

#include "../content_provider.h"
#include "../memo_provider.h"
#include <common.h>
#include <tg_utility.h>

static int memo_provider_test_suit_init(void)
{

    return 0;
}
static int memo_provider_test_suit_clean(void)
{
  //  tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void memo_provider_insert_test()  
{	
	INT32 ret = 0;
	INT32 i = 0;
	CHAR caption[30];
	CHAR content[100];
	
	ContentProvider* cp = content_provider_open(MEMO_PROVIDER_ID, FALSE);
	ContentValues* values = NULL;
	return_if_fail(cp);
	ret=content_provider_delete(cp,MEMO_PROVIDER_DESTINATION_LIST,NULL);
	
	values = content_values_create();
	if(!values)
	{
		CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
		return;
	}
	for(i=0;i<20;i++)
	{
		sprintf(caption,"caption_%d",i);
		content_values_put_string(values,MEMO_PROVIDER_COL_CAPTION,caption);
		sprintf(content,"content_%d",i);
		content_values_put_string(values,MEMO_PROVIDER_COL_CONTENT,content);

		ret = content_provider_insert(cp,MEMO_PROVIDER_DESTINATION_LIST,"caption,content",2,values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	content_values_destroy(values);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

	
}

static void memo_provider_query_test()
{	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(MEMO_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, MEMO_PROVIDER_DESTINATION_LIST, "ID,caption,content,time",NULL,"time",NULL);
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"caption");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"content");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"time");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 datetime;
				CHAR* content =NULL;
				CHAR* caption =NULL;
				CHAR* time =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				caption= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( caption);

				content= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( content);

				time= cursor_get_string(cursor,3);
				CU_ASSERT_PTR_NOT_NULL( time);
				db_log("caption:%s;content:%s;time:%s",caption,content,time);

			
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}

static void memo_provider_query_v2_test()
{	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(DB_WHERE_CLAUSE, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, MEMO_PROVIDER_DESTINATION_LIST, "ID,caption,content,time",NULL,"time","10,3");
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"caption");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"content");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"time");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 datetime;
				CHAR* content =NULL;
				CHAR* caption =NULL;
				CHAR* time =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				caption= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( caption);

				content= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( content);

				time= cursor_get_string(cursor,3);
				CU_ASSERT_PTR_NOT_NULL( time);
				db_log("caption:%s;content:%s;time:%s",caption,content,time);

			
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}
INT32 memo_provider_query_v2_asyn_cb(ContentProvider* thiz,INT32 error,void* user_data,void* para)
{
	Cursor* cursor = (Cursor*)para;
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"caption");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"content");
		key_name = cursor_get_column_name(cursor,3);
		CU_ASSERT_STRING_EQUAL(key_name,"time");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				INT64 datetime;
				CHAR* content =NULL;
				CHAR* caption =NULL;
				CHAR* time =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				caption= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( caption);

				content= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( content);

				time= cursor_get_string(cursor,3);
				CU_ASSERT_PTR_NOT_NULL( time);
				db_log("caption:%s;content:%s;time:%s",caption,content,time);

			
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	cursor_destroy(cursor);
	content_provider_close(thiz);
    return 0;
}
static void memo_provider_query_async_test()
{
	
	INT32 ret = 0;
	INT32 i = 0;
	
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(MEMO_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	ret=content_provider_query_asyn(cp, MEMO_PROVIDER_DESTINATION_LIST, "ID,caption,content,time",NULL,"time","10,3",memo_provider_query_v2_asyn_cb,NULL);
	
	if(ret != DB_TODO)
	{
		CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	}
	
	
	
}
static void memo_provider_update_test()
{
}

static void memo_provider_delete_test()
{	

	INT32 ret = 0;
	INT32 i = 0;

	ContentProvider* cp = content_provider_open(MEMO_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	ret=content_provider_delete(cp, MEMO_PROVIDER_DESTINATION_LIST, NULL);
	CU_ASSERT_EQUAL(ret,DB_OK);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}

static void memo_provider_get_count_test()
{
	INT32 count = 0;
	INT32 i = 0;

	ContentProvider* cp = content_provider_open(MEMO_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	count=content_provider_get_count(cp, MEMO_PROVIDER_DESTINATION_LIST);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}
static CU_TestInfo memo_provider_testcases[] =
{ 
    {"memo_provider_insert_test",memo_provider_insert_test},
    {"memo_provider_query_test",memo_provider_query_test},  //update
    {"memo_provider_query_v2_test",memo_provider_query_v2_test},
//    {"memo_provider_update_test:", memo_provider_update_test},  //delete
    {"memo_provider_get_count_test:", memo_provider_get_count_test},  //delete
//    {"memo_provider_insert_test",memo_provider_insert_test},
	{"memo_provider_query_async_test:", memo_provider_query_async_test},  //delete
    CU_TEST_INFO_NULL
};


static CU_SuiteInfo memo_provider_suites[] = {
    {"Testing memo_provider_testcases:", NULL, memo_provider_test_suit_clean, memo_provider_testcases},
    CU_SUITE_INFO_NULL
};


static void memo_provider_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(memo_provider_suites))
    {
        return;
    }
}

void memo_provider_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        memo_provider_add_tests();
        CU_set_output_filename("memo_provider_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}

