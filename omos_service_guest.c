#include "omos.h"

void service_guest(PGconn *__soc){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    pthread_t selfId = pthread_self();

    while(1){
        sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"UCHG\"，\"RESESRVE\"です．\n操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm_1);
            if(strcmp(comm_1, UCHG) == 0){
                if(userChange() == -1){
                    sprintf(sendBuf, "会員情報の変更に失敗しました%s", ENTER);
                }
            }else if(strcmp(comm_1, RESERVE) == 0){
                while(1){
                    sprintf(sendBuf, "RESERVEの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"RECHECK\"，\"REDEL\"です．%s", ENTER, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        sscanf(recvBuf, "%s", comm_1);
                        if(strcmp(comm_1, REDEL == 0)){
                            if(correctCheck() != 0){     //集計確認正常完了: 0
                                sprintf(sendBuf, "集計確認に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else if(strcmp(comm_1, REDEL) == 0){
                            if(saleCheck() == 0){     //売上確認正常完了: 0
                                sprintf(sendBuf, "売上確認に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else{
                            sprintf(sendBuf, "コマンドを正しく入力してください%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                    }
                }
            }else if(strcmp(comm_1, END) == 0){
                sprintf(sendBuf, "操作を終了し，ログイン画面に戻ります%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }
        }
    }
}