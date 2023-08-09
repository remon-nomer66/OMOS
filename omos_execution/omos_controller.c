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
    char comm[BUFSIZE];

    recvLen = flag = f_login = 0;
    table_num = kitchen_y_n = 0;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, threadParam->soc);

    while(1){
        if(kitchen_y_n == 0){
            userCheck(selfId, threadParam->con, threadParam->soc, recvBuf, sendBuf, u_info);              //ユーザー認証
        }
        printf("user_id: %d, auth: %d, store_id: %d\n", u_info[0], u_info[1], u_info[2]);
        printf("s_id: %d, t_id: %d, k/n: %d\n", s_info[0], s_info[1], s_info[2]);
        if(s_info[1] != 0){
            service_table(selfId, threadParam->con, threadParam->soc, recvBuf, sendBuf, u_info, s_info);          //卓の処理
        }//else if(s_info[2] != 0){
            //kitchen(threadParam->con, threadParam->soc, s_info[0]);        //キッチンの処理
        //}
        else if(u_info[1] == AGUEST){
            service_guest(selfId, threadParam->con, threadParam->soc, recvBuf, sendBuf, u_info);                //ゲストの処理
        }else if(u_info[1] != ANUREG){
            service_employee(selfId, threadParam->con, threadParam->soc, recvBuf, sendBuf, u_info, s_info, reg);  //店員の処理
            kitchen_y_n = reg[0];
            table_num = reg[1];
            printf("%d\n", kitchen_y_n);
        }
        if(table_num == 0 && kitchen_y_n == 0){
            sprintf(sendBuf, "端末を終了する場合は\"END\"と入力してください%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(threadParam->soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
            if(recvLen > 0){
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, END) == 0){
                    sprintf(sendBuf, "%s END%s%s", OK_STAT, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(threadParam->soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }
            }
        }
    }

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
