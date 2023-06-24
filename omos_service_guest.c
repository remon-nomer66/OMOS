#include "omos.h"

void service_guest(int __soc){
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
                while(1){
                    sprintf(sendBuf, "RESERVEの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"RECHECK\"，\"REDEL\"です．%s", ENTER, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        sscanf(recvBuf, "%s", comm);
                        if(strcmp(comm, RESERVE) == 0){
                            if(reserveReg() != 0){     //予約確認正常完了: 0
                                sprintf(sendBuf, "予約に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else if(strcmp(comm, RECHECK) == 0){
                            if(reserveCheck() != 0){     //予約確認正常完了: 0
                                sprintf(sendBuf, "予約確認に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else if(strcmp(comm, REDEL) == 0){
                            if(reserveDel() == 0){     //予約削除正常完了: 0
                                sprintf(sendBuf, "予約削除に失敗しました．%s", ENTER);
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