#include "omos.h"

/**
 * OMOSメイン関数(スレッド関数)
 * [ARGUMENT]
 *      __arg   : スレッド引数構造体(ThreadParameter型)
 * [RETURN]
 *      NONE
 */
void *omos_controller(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    pthread_t selfId;
    int cnt, kitchen_y_n;
    int table_num;
    int reg[2];
    int flag, f_login;
    int store_id;
    int u_info[3]; //[0]: アカウントID(user_id)，[1]: 権限, [2]: 店員の店舗番号(store_id)
    int s_info[3];  //[0]: 端末の店舗番号，[1]: 卓番号, [2]: kitchen(y/n)

    recvLen = flag = f_login = 0;
    table_num = kitchen_y_n = 0;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, threadParam->soc);

    while(1){
        service_user(selfId, theradParam->con, threadParam->soc, recvBuf, sendBuf, u_info);              //ユーザー認証
        if(table_num != 0){
            service_table(selfId, theradParam->con, threadParam->soc, u_info, s_info);          //卓の処理
        }else if(kitchen_y_n != 0){
            service_kitchen(selfId, theradParam->con, threadParam->soc, u_info, s_info);        //キッチンの処理
        }else if(u_info[1] == AGUEST){
            service_guest(selfId, theradParam->con, threadParam->soc, u_info);                //ゲストの処理
        }else if(u_info[1] != ANUREG){
            service_employee(selfId, theradParam->con, threadParam->soc, u_info, s_info);  //店員の処理
            kitchen_y_n = reg[0];
            table_num = reg[1];
        }
    }

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
