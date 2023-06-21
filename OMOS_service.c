#include "omos.h"

/**
 * OMOSメイン関数(スレッド関数)
 * [ARGUMENT]
 *      __arg   : スレッド引数構造体(ThreadParameter型)
 * [RETURN]
 *      NONE
 */
void *omos_server(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    char comm_1[BUFSIZE], comm_2[BUFSIZE];
    int perm_1, perm_2, perm_3;
    pthread_t selfId;
    int cnt, auth, count, flag, kitchen_y_n, i;
    int table_num;
    int register[2];

    recvLen = count = flag = 0;
    i = 0;

    table_num = kitchen_y_n = 0;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, threadParam->soc);

    while(1){
        auth = user_check(threadParam->soc);
        if(table_num != 0){
            service_table(threadParam->soc, auth);
        }else if(kitchen_y_n != 0){
            service_kitchen(threadParam->soc, auth);
        }else if(auth == AGUEST){
            service_guest(threadParam->soc);
        }else{
            service_employee(threadParam->soc, auth, register);
            kitchen_y_n = register[0];
            table_num = register[1];
        }
    }

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadPram);
}