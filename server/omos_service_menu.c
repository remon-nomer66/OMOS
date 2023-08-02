#include "omos.h"
#include "menu.h"

void service_menu(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char comm[BUFSIZE];  //コマンド
    int recvLen, sendLen;
    int flag = 0;

    while(1){
        sendLen = sprintf(sendBuf, "MENUの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"MREG\"，\"MDEL\"，\"MCHG\"です．%sMENUを終了するには\"END\"と入力してください．%s%s", ENTER, ENTER, ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, MREG) == 0){
                menuReg(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if(strcmp(comm, MDEL) == 0){
                menuDel(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if(strcmp(comm, MCHG) == 0){
                menuChg(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "MENUを終了します%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }else{
                sprintf(sendBuf, "%s。%d%s", ER_STAT, E_CODE_1701, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }else{
            sprintf(sendBuf, "%s。%d%s", ER_STAT, E_CODE_1701, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }
    return;
}