#include "omos.h"

void service_guest(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char comm[BUFSIZE];
    int recvLen, sendLen;

    while(1){
        sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"UCHG\"，\"RESESRVE\"です．\n操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, UCHG) == 0){
                userChg();
            }else if(strcmp(comm, RESERVE) == 0){
                reserve(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if(strcmp(comm, END) == 0){
                break;
            }
        }
    }
}