#include "omos.h"

void *test_controller(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int sendLen, recvLen;
    pthread_t selfId;
    PGconn *con = threadParam->con;
    int soc = threadParam->soc;
    char comm[BUFSIZE];
    int u_info[3] = {1001, 3, 10};  //[0]: アカウントID(user_id)，[1]: 権限, [2]: 店舗番号(store_id)
    int s_info[3] = {10, 2, 0};     //[0]: 店舗番号，[1]: 卓番号, [2]: kitchen(y/n)

    selfId = pthread_self();
    printf("[C_THREAD %ld] TEST CONTROLLER START (%d)\n", selfId, soc);

    while(1){
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

	    test_func(selfId, con, soc, recvBuf, sendBuf);
	}else{
	  break;
	}
    }//END while()

    printf("[C_THREAD %ld] TEST CONTROLLER END (%d)\n\n", selfId, threadParam->soc);
    
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
