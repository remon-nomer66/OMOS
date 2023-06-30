#include "test.h"

void test_func(PGconn *__con, int __soc){
    char sendBuf[BUFSIZE], recvBuf[BUFSIZE];
    int sendLen, recvLen;
    int param1, param2;
    int cnt;
    pthread_t selfId;

    selfId = pthread_self();

    while(1){
        sprintf(sendBuf, "足し合わせたい2つの数値を「1 23」の形で入力してください．%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            cnt = sscanf(recvBuf, "%d %d", &param1, &param2);
            if(cnt == 2){
                param1 += param2;
                sprintf(sendBuf, "結果: %d%s", param1, ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }
        }
    }
    return;
}