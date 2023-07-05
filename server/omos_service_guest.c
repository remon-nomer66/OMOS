#include "omos.h"

void service_guest(PGconn *__con, int __soc, int *__auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    char comm[BUFSIZE];
    int recvLen, sendLen;
    pthread_t selfId = pthread_self();

    while(1){
        sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"UCHG\"，\"RESESRVE\"です．\n操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, UCHG) == 0){
                if(userChg() == -1){
                    sprintf(sendBuf, "会員情報の変更に失敗しました%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, RESERVE) == 0){
                reserve(__con, __soc, __auth);
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "操作を終了し，ログイン画面に戻ります%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }
        }
    }
}