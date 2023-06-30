#include "test.h"

/**
 * TESTメイン関数(スレッド関数)
 * [ARGUMENT]
 *      __arg   : スレッド引数構造体(ThreadParameter型)
 * [RETURN]
 *      NONE
 */
void *test_service(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char sendBuf[BUFSIZE], recvBuf[BUFSIZE];
    int sendLen, recvLen;
    char comm[BUFSIZE];
    pthread_t selfId;
    PGconn *__con = threadParam->con;
    int __soc = threadParam->soc;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, __soc);

    while(1){
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, TEST) == 0){
                test_func(__con, __soc);  //任意の関数，引数に変更
                sprintf(sendBuf, "スレッドを終了しますか？%s終了する場合は\"YES\"，終了しない場合は何らかの文字列を入力して下さい．%s", ENTER, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                    sscanf(recvBuf, "%s", comm);
                    if(strcmp(comm, YES) == 0){
                        break;
                    }
                }
            }else{
                sprintf(sendBuf, "入力されたコマンドが誤っているようです%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }

    sprintf(sendBuf, "END");
    sendLen = strlen(sendBuf);
    send(threadParam->soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
