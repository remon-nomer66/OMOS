#include "omos.h"

void reserve_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info){
    char comm[BUFSIZE];
    int recvLen, sendLen;

    while(1){
        sprintf(sendBuf, "%s %d%s", OK_STAT, 3, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        sprintf(sendBuf, "RESERVEの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"RECHECK\"，\"REREG\"，\"RECHG\"，\"REDEL\"です．%sRESERVEから抜ける場合は\"END\"を入力してください%s%s", ENTER, ENTER, ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, RECHECK) == 0){
                reserveShow_s(selfId, con, soc, recvBuf, sendBuf, s_info);
            }else if(strcmp(comm, REREG) == 0){
                reserveReg_s(selfId, con, soc, recvBuf, sendBuf, s_info);
                printf("flag\n");
            }else if(strcmp(comm, RECHG) == 0){
                reserveChg_s(selfId, con, soc, recvBuf, sendBuf, s_info);
            }else if(strcmp(comm, REDEL) == 0){
                reserveDel_s(selfId, con, soc, recvBuf, sendBuf, s_info);
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "%s %d%s", OK_STAT, 3, ENTER);
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
        }else{
            sprintf(sendBuf, "コマンドを正しく入力してください%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }
    return;
}