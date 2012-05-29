#include "CUnit.h"
#include "Automated.h"

#include "../content_provider.h"
#include "../contact_provider.h"
#include <common.h>
#include <tg_utility.h>

static int contact_provider_test_suit_init(void)
{

    return 0;
}
static int contact_provider_test_suit_clean(void)
{
  //  tg_heap_traverse(FILE_PRINT);
    return 0;
}

static void contact_provider_insert_test()  
{	
	INT32 ret = 0;
	INT32 i = 0;
	CHAR name[30];
	CHAR number[20];
	
	ContentProvider* cp = content_provider_open(CONTACT_PROVIDER_ID, FALSE);
	ContentValues* values = NULL;
	return_if_fail(cp);
	ret=content_provider_delete(cp,CONTACT_PROVIDER_DESTINATION_CONTACTS,NULL);
	
	values = content_values_create();
	if(!values)
	{
		CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
		return;
	}
	for(i=0;i<1000;i++)
	{
		sprintf(name,"contact_name_%d",i);
		content_values_put_string(values,"name",name);
		sprintf(number,"13511110%d",i);
		content_values_put_string(values,"number",number);
		ret = content_provider_insert(cp,CONTACT_PROVIDER_DESTINATION_CONTACTS,"name,number",2,values);
		CU_ASSERT_EQUAL(ret,DB_OK);
	}
	content_values_destroy(values);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

	
}

static void contact_provider_query_test()
{	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(CONTACT_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, CONTACT_PROVIDER_DESTINATION_CONTACTS, "ID,name,number",NULL,"name",NULL);
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"name");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"number");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				CHAR* number =NULL;
				CHAR* name =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				name= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( name);

				number= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( number);
			
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}

static void contact_provider_query_v2_test()
{	
	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(CONTACT_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	cursor=content_provider_query(cp, CONTACT_PROVIDER_DESTINATION_CONTACTS, "ID,name,number",NULL,"name","10,3");
	CU_ASSERT_PTR_NOT_NULL(cursor);
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"name");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"number");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				CHAR* number =NULL;
				CHAR* name =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				name= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( name);

				number= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( number);
			
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	cursor_destroy(cursor);
	
}
INT32 content_provider_query_v2_asyn_cb(ContentProvider* thiz,INT32 error,void* user_data,void* para)
{
	Cursor* cursor = (Cursor*)para;
	if(cursor)
	{
		INT32 read_num = cursor_get_count(cursor);
		INT32 i = 0;
		const CHAR* key_name = cursor_get_column_name(cursor,0);
		CU_ASSERT_STRING_EQUAL(key_name,"ID");
		key_name = cursor_get_column_name(cursor,1);
		CU_ASSERT_STRING_EQUAL(key_name,"name");
		key_name = cursor_get_column_name(cursor,2);
		CU_ASSERT_STRING_EQUAL(key_name,"number");
		if(read_num>0)
		{
			do
			{
				INT64 id;
				CHAR* number =NULL;
				CHAR* name =NULL;
				CU_ASSERT_EQUAL(cursor_get_int(cursor,0,&id),TRUE);
				name= cursor_get_string(cursor,1);
				CU_ASSERT_PTR_NOT_NULL( name);

				number= cursor_get_string(cursor,2);
				CU_ASSERT_PTR_NOT_NULL( number);
			
				i++;
				
			}while(cursor_move_to_next(cursor));
		}

		
	   
	}
	cursor_destroy(cursor);
	content_provider_close(thiz);
}
static void contact_provider_query_async_test()
{
	
	INT32 ret = 0;
	INT32 i = 0;
	
	//DatabaseWhereClause* clause = db_clause_create(DB_WHERE_CLAUSE,CHAR* statement,0);
	ContentProvider* cp = content_provider_open(CONTACT_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	ret=content_provider_query_asyn(cp, CONTACT_PROVIDER_DESTINATION_CONTACTS, "ID,name,number",NULL,"name","10,3",content_provider_query_v2_asyn_cb,NULL);
	
	if(ret != DB_TODO)
	{
		CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);
	}
	
	
	
}
static void contact_provider_update_test()
{
}

static void contact_provider_delete_test()
{	

	INT32 ret = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(CONTACT_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	ret=content_provider_delete(cp, CONTACT_PROVIDER_DESTINATION_CONTACTS, NULL);
	CU_ASSERT_EQUAL(ret,DB_OK);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}

static void contact_provider_get_count_test()
{
	INT32 count = 0;
	INT32 i = 0;
	Cursor* cursor = NULL;
	ContentProvider* cp = content_provider_open(CONTACT_PROVIDER_ID, FALSE);
	return_if_fail(cp);
	count=content_provider_get_count(cp, CONTACT_PROVIDER_DESTINATION_CONTACTS);
	CU_ASSERT_EQUAL(content_provider_close(cp),DB_OK);

}
static CU_TestInfo contact_provider_testcases[] =
{ 
    {"contact_provider_insert_test",contact_provider_insert_test},
    {"contact_provider_query_test",contact_provider_query_test},  //update
    {"contact_provider_query_v2_test",contact_provider_query_v2_test},
//    {"contact_provider_update_test:", contact_provider_update_test},  //delete
    {"contact_provider_get_count_test:", contact_provider_get_count_test},  //delete
//    {"contact_provider_insert_test",contact_provider_insert_test},
	{"contact_provider_query_async_test:", contact_provider_query_async_test},  //delete
    CU_TEST_INFO_NULL
};


static CU_SuiteInfo contact_provider_suites[] = {
    {"Testing contact_provider_testcases:", NULL, contact_provider_test_suit_clean, contact_provider_testcases},
    CU_SUITE_INFO_NULL
};


static void contact_provider_add_tests(void)
{
    CU_get_registry();
    if (CU_is_test_running())
        return;


    if (CUE_SUCCESS != CU_register_suites(contact_provider_suites))
    {
        return;
    }
}

void contact_provider_test_run()
{
    if (CU_initialize_registry())
    {
        return;
    }
    else
    {
        contact_provider_add_tests();
        CU_set_output_filename("contact_provider_test_output");
        CU_list_tests_to_file();
        CU_automated_run_tests();
        CU_cleanup_registry();
    }
}

