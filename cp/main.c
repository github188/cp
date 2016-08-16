/*****************************************************
 *
 * 主函数 & 启动
 *
 *****************************************************/
#include "header.h"

#define IS_DAEMON_EXIST			0					// 精灵线程
#define PORTAL_TEST_THREAD		0
#define SQL_TEST_THREAD			0

/** 
 *@brief  程序主体函数 由子进程运行
 */
void run()
{
#if IS_DAEMON_EXIST 
	//关闭父进程创建的log文件
	logs_destroy();
#endif
	//创建子进程log文件
	if( logs_init("logs") ){
		printf("logs_init error!!!!\n");
		exit(-1);
	}
	
	//打印程序开启信息
	xyprintf(0, "\n\t\t\t*************************************************************************\n\
			*************************************************************************\n\
			****                                                                 ****\n\
			****                      The cp.out is running!                     ****\n\
			****                          Pid is %d                            ****\n\
			****                                                                 ****\n\
			*************************************************************************\n\
			*************************************************************************\n",
			getpid() );
	RADIUS_SECRET_LEN = strlen(RADIUS_SECRET);
	xyprintf(0, "RADIUS_SECRET is %s, len is %d", RADIUS_SECRET, RADIUS_SECRET_LEN);
	pthread_t pt;
	/****************到时提出xiancheng**************************************************/
	if( pthread_create(&pt, NULL, loop_deadline_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
#if 0
	/****************平台连接监视线程**************************************************/
	if( pthread_create(&pt, NULL, platform_conn_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
	/****************1812端口**********************************************************/
	if( pthread_create(&pt, NULL, radius12_conn_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
	/****************1813端口**********************************************************/
	if( pthread_create(&pt, NULL, radius13_conn_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
	/****************portal连接********************************************************/
	if( pthread_create(&pt, NULL, portal_conn_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
	/**********************************************************************************/
#if SQL_TEST_THREAD
	if( pthread_create(&pt, NULL, sql_test_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
#endif
#if PORTAL_TEST_THREAD
	if( pthread_create(&pt, NULL, portal_test_thread, NULL) != 0 ){
		xyprintf(errno, "PTHREAD_ERROR: %s %d -- pthread_create()", __FILE__, __LINE__);
	}
#endif

#endif
	exec_sql_test();
	while(1){
		sleep(100);
	}
	//不会执行到的一步
	//pthread_mutex_destroy(&gv_authenticate_list_lock);
}

/** 
 *@brief  主函数 启动子进程中运行程序主体 父进程监控主进程的运行状态
 */
int main(int argc, char** argv)
{
#if IS_DAEMON_EXIST 
	if( logs_init("wizard_logs") ){
		printf("DATA_ERROR:%s %d -- logs_init error!!!!\n",__FILE__, __LINE__);
		exit(-1);
	}

	// 守护进程（父进程）
	int status;
	for(;;){
		if( 0 == fork() ){
			run();
		}
		xyprintf(0, "ELF_THREAD:The child process is running!");
	    waitpid(-1, &status, 0);
		xyprintf(0, "ELF_THREAD:The child process is dead!");
		if(WIFSIGNALED(status)) {
			xyprintf(0, "ELF_THREAD:The child process is dead! Signal is %d", WTERMSIG(status) );
        }
	}
#else
	run();
#endif
}
