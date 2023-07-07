#include "omos.h"
#include "test.h"

int test_func(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf){
    int sendLen, recvLen;
    int param1, param2;
    int cnt;
    char buf[BUFSIZE];

    while(1){
        sprintf(sendBuf, "足し合わせたい2つの数値を「1 23」の形で入力してください%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s", selfId, sendBuf);

	sendLen = sprintf(sendBuf, "%s", DATA_END);
	send(soc, sendBuf, sendLen, 0);
	printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            cnt = sscanf(recvBuf, "%d %d", &param1, &param2);
            if(cnt == 2){
                param1 += param2;
                sprintf(sendBuf, "結果: %d%s%s", param1, ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }
        }
        if(recvLen == 0 || cnt != 2){
	  sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1001, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return -1;
        }
    }
    return 0;
}
