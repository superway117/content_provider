
#ifndef _MMSSMS_DB_HELPER_H_
#define _MMSSMS_DB_HELPER_H_


#include <db_wrapper.h>


//define col name
#define SMS_DB_PATH "/nvram/mmssms.db"



#define SMS_TABLE_NAME "sms_tb"

#define SMS_MESSAGE_TYPE_INBOX				  	0    
#define SMS_MESSAGE_TYPE_SENT				  		1  
#define SMS_MESSAGE_TYPE_OUTBOX				  	2  
#define SMS_MESSAGE_TYPE_DRAFT				  	3    			//THE DRAFT BOX must 3,or i need to change the sql statement
#define SMS_MESSAGE_TYPE_FAILED				  	4   			//failed outgoing message
#define SMS_MESSAGE_TYPE_QUEUED				  	5  			//for message to send later

#define SMS_TABLE_COL_ID "sms_id"  
#define SMS_TABLE_COL_THREAD_ID "thread_id"  
#define SMS_TABLE_COL_TYPE "type"  
#define SMS_TABLE_COL_ADDRESS "address"  
#define SMS_TABLE_COL_DATE "date" 
#define SMS_TABLE_COL_PROTOCAL "protocol" 
#define SMS_TABLE_COL_READ "read" 
#define SMS_TABLE_COL_STATUS "status" 
#define SMS_TABLE_COL_REPLY_PATH_PRESENT "reply_path_present" 
#define SMS_TABLE_COL_BODY "body" 


#define SMS_THREADS_TABLE_NAME "threads_tb"  //this is a common table

#define SMS_THREADS_TABLE_COL_ID 					"thread_id"  
#define SMS_THREADS_TABLE_COL_ADDRESS 			"canonical_address"  
#define SMS_THREADS_TABLE_COL_DATE 				"date"  
#define SMS_THREADS_TABLE_COL_MESSAGE_COUNT 	"message_count"  
#define SMS_THREADS_TABLE_COL_UNREAD_COUNT 	"unread_count"  
#define SMS_THREADS_TABLE_COL_FAILED_COUNT 		"failed_count"  
#define SMS_THREADS_TABLE_COL_CACHED_NAME		 "cached_name"  

extern INT32 sms_create_tables(DatabaseHandle* db_handle);

extern INT32 sms_create_common_tables(DatabaseHandle* db_handle);

extern INT32 sms_create_triggers(DatabaseHandle* db_handle);

extern INT32 sms_create_index(DatabaseHandle* db_handle);

extern INT32 sms_drop_all(DatabaseHandle* db_handle);

extern INT32 sms_update_thread(DatabaseHandle* db_handle, INT64 thread_id);

extern INT32 sms_update_all_threads(DatabaseHandle* db_handle);

extern INT32 sms_get_or_create_thread_id(DatabaseHandle* db_handle,const CHAR* address,INT64* thread_id_output);

#endif
