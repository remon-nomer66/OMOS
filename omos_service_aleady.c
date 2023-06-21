#include "omos.h"

#define GUEST       1
#define EMPLOYEE    2

void service_table(PGconn *__soc, int __auth){
    int tel, auth, cnt;
    char comm[BUFSIZE];
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;
    pthread_t selfId = pthread_self();

    if(__auth != AMGR && __auth != ACLERK){
        while(1){
            sprintf(sendBuf, "社員としてログインしますか？社員として利用する場合は\"YES\"，お客様として利用する場合は\"NO\"を入力してください．%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            if(recvLen != 0){
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm_1);
                if(strcmp(comm_1, YES) == 0){
                    flag = EMPLOYEE;
                    sprintf(sendBuf, "社員としてログインします%s", ENTER);
                    break;
                }else if(strcmp(comm_1, NO) == 0){
                    flag = GUEST;
                    sprintf(sendBuf, "お客様としてログインします%s", ENTER);
                    break;
                }
            }
        }
    }
    sendLen = strlen(sendBuf);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    if(flag == EMPLOYEE){
        sprintf(sendBuf, "卓を削除するには\"TDEL\"を入力してください%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        if(tableDel() != -1){
            sprintf(sendBuf, "卓の削除に失敗しました%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }else{
        while(1){
            sprintf(sendBuf, "注文する場合は\"ORDER\"，履歴を確認する場合は\"HIST\"，会計を行う場合は\"PAY\"を入力してください．なお，注文する場合は\"ORDER (商品番号) (注文個数)\"で入力してください．%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE); 
            if(recvLen != 0){
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm_1);
                if(strcmp(comm_1, ORDER) == 0){
                    if(order() != -1){
                        sprintf(sendBuf, "注文に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else if(strcmp(comm_1, HISTORY) == 0){
                    if(history() != -1){
                        sprintf(sendBuf, "履歴の確認に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else if(strcmp(comm_1, PAY) == 0){
                    while(1){   //店員照会
                        while(1){
                            sprintf(sendBuf, "会計を開始します．店員は自身のアカウントでログインしてください．%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                            if(recvLen > 0){
                                recvBuf[recvLen - 1] = '\0';
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                cnt = sscanf(recvBuf, "%d", tel);
                                if(cnt == 1){
                                    break;
                                }
                            }
                        }
                        while(1){
                            sprintf(sendBuf, "パスワードを入力してください．%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                            if(recvLen > 0){
                                recvBuf[recvLen - 1] = '\0';
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                cnt = sscanf(recvBuf, "%s", comm);
                                if(cnt == 1){
                                    if((auth = userCheck()) != -1){   //戻り値-1でなければなら会員として正しい
                                        break;
                                    }else{
                                        sprintf(sendBuf, "ログイン画面に戻ります．%s", ENTER);
                                        sendLen = strlen(sendBuf);
                                        send(__soc, sendBuf, sendLen, 0);
                                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                        break;
                                    }
                                    if(auth != AMGR && auth != ACLERK){
                                        sprintf(sendBuf, "ログイン画面に戻ります．%s", ENTER);
                                        sendLen = strlen(sendBuf);
                                        send(__soc, sendBuf, sendLen, 0);
                                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if(pay() != -1){
                        sprintf(sendBuf, "会計に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else{
                    sprintf(sendBuf, "無効なコマンドです%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }
        }
    }
}

void service_kitchen(PGconn *__soc, int __auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;
    pthread_t selfId = pthread_self();

    if(__auth == AMGR || __auth == ACLERK){
        while(1){
            sprintf(sendBuf, "完了した商品の登録は\"KFLAG\"，キッチン端末を終了するには\"KDEL\"を入力してください%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE); 
            if(recvLen != 0){
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm_1);
                if(strcmp(comm_1, KFLAG) == 0){
                    if(kitchenFlag() != -1){
                        sprintf(sendBuf, "完了した商品の登録に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else if(strcmp(comm_1, END) == 0){
                    if(kitchenDel() != -1){ //=======キッチン終了関数kitchenDel
                        break;
                    }else{
                        sprintf(sendBuf, "キッチン端末の終了に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else{
                    sprintf(sendBuf, "無効なコマンドです%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }   
        }
    }else if(__auth != AMGR &&  __auth != ACLERK){ //キッチン端末を操作しているのが店長でも店員でもない場合
        sprinf(sendBuf, "権限がない端末を操作しています．\nログイン画面に戻ります．%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }
}