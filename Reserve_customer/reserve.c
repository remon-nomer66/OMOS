#include "omos.h"

void reserve(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char comm[BUFSIZE];
    int recvLen, sendLen;

    while(1){
        sprintf(sendBuf, "RESERVEの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"RECHECK\"，\"REDEL\"です．%sRESERVEから抜ける場合は\"END\"を入力してください%s", ENTER, ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, RESERVE) == 0){
                reserveReg(selfId, con, soc, recvBuf, sendBuf, u_info);     //予約確認正常完了: 0
            }else if(strcmp(comm, RECHECK) == 0){
                reserveChange(selfId, con, soc, recvBuf, sendBuf, u_info);     //予約確認正常完了: 0
            }else if(strcmp(comm, REDEL) == 0){
                reserveDel(selfId, con, soc, recvBuf, sendBuf, u_info);     //予約削除正常完了: 0
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "RESERVEを終了します%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }else{
                sprintf(sendBuf, "コマンドを正しく入力してください%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }
    return;
}