#include "omos.h"

/**
 * OMOSメイン関数(スレッド関数)
 * [ARGUMENT]
 *      __arg   : スレッド引数構造体(ThreadParameter型)
 * [RETURN]
 *      NONE
 */
void *omos_service(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    pthread_t selfId;
    int cnt, kitchen_y_n;
    int table_num;
    int reg[2];
    int u_info[3]; //[0]: アカウントID(user_id)，[1]: 権限, [2]: 店舗番号(store_id)
    int flag, f_login;
    int store_id;
    int s_info[3];  //[0]: 店舗番号，[1]: 卓番号, [2]: kitchen(y/n)

    recvLen = flag = f_login = 0;
    table_num = kitchen_y_n = 0;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, threadParam->soc);

    while(1){
        service_user(theradParam->con, threadParam->soc, u_info);              //ユーザー認証
        if(table_num != 0){
            if(service_table(theradParam->con, threadParam->soc, u_info, s_info) == INIT){          //卓の処理
                store_id = table_num = 0;
            }
        }else if(kitchen_y_n != 0){
            service_kitchen(theradParam->con, threadParam->soc, u_info);        //キッチンの処理
        }else if(auth[0] == AGUEST){
            service_guest(theradParam->con, threadParam->soc, u_info);                //ゲストの処理
        }else if(auth[0] != ANUREG){
            service_employee(theradParam->con, threadParam->soc, u_info, s_info);  //店員の処理
            kitchen_y_n = reg[0];
            table_num = reg[1];
        }
    }

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
