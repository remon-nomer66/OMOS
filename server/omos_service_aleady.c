#include "omos.h"

#define FGUEST       1
#define FEMPLOYEE    2

<<<<<<< Updated upstream
int service_table(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
=======
int service_table(PGconn *__con, int __soc, int *__auth, int __table_num, int __store_id){
>>>>>>> Stashed changes
    int tel, cnt;
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;
<<<<<<< Updated upstream

    if(u_info[1] != AMGR && u_info[1] != ACLERK){
=======
    pthread_t selfId = pthread_self();
    int e_auth[2];

    if(__auth[0] != AMGR && __auth[0] != ACLERK){
>>>>>>> Stashed changes
        while(1){
            sprintf(sendBuf, "社員としてログインしますか？社員として利用する場合は\"YES\"，お客様として利用する場合は\"NO\"を入力してください．%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
	        recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen != 0){
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, YES) == 0){
                    flag = FEMPLOYEE;
                    sprintf(sendBuf, "社員としてログインします%s", ENTER);
                    break;
                }else if(strcmp(comm, NO) == 0){
                    flag = FGUEST;
                    sprintf(sendBuf, "お客様としてログインします%s", ENTER);
                    break;
                }
            }
        }
    }
    sendLen = sprintf(sendBuf, "%s", DATA_END);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    if(flag == FEMPLOYEE){
<<<<<<< Updated upstream
        if(tableDel() != -1){
            s_info[0] = 0;  //店舗番号初期化
            s_info[1] = 0;  //卓番号初期化
            s_info[2] = 0;  //キッチンか否かのフラグ(一応0にしておく)
            return 0;
        }else{
=======
        /* sprintf(sendBuf, "卓を削除するには\"TDEL\"を入力してください%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); */
        if(tableDel() != -1){
            sprintf(sendBuf, "卓の削除に失敗しました%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
>>>>>>> Stashed changes
            return -1;
        }
        return INIT;
    }else{
<<<<<<< Updated upstream
        if(order(user_id, soc, __auth, __table_num, __store_id) != -1){
            return 0;
        }else{
            return -1;
=======
        while(1){
            sprintf(sendBuf, "注文する場合は\"ORDER\"，履歴を確認する場合は\"HIST\"，会計を行う場合は\"PAY\"を入力してください．なお，注文する場合は\"ORDER (商品番号) (注文個数)\"で入力してください．%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); 
            if(recvLen != 0){
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, ORDER) == 0){
		            if(order(__con, __soc, __auth, __table_num, __store_id) != -1){
		                printf("order\n");
                        sprintf(sendBuf, "注文に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else if(strcmp(comm, HISTORY) == 0){
		            if(history() != -1){
                        printf("history\n");
                        sprintf(sendBuf, "履歴の確認に失敗しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else if(strcmp(comm, PAY) == 0){
                    while(1){   //店員照会
                        while(1){
                            sprintf(sendBuf, "会計を開始します．店員は自身のアカウントでログインしてください．%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                            if(recvLen > 0){
                                recvBuf[recvLen - 1] = '\0';
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                cnt = sscanf(recvBuf, "%d", &tel);
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
                            recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                            if(recvLen > 0){
                                recvBuf[recvLen - 1] = '\0';
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                cnt = sscanf(recvBuf, "%s", comm);
                                if(cnt == 1){
                                    if(userCheck() != -1){   //戻り値-1でなければ会員として正しい
                                        break;
                                    }else{
                                        sprintf(sendBuf, "ログイン画面に戻ります．%s", ENTER);
                                        sendLen = strlen(sendBuf);
                                        send(__soc, sendBuf, sendLen, 0);
                                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                        break;
                                    }
                                    if(e_auth[0] != AMGR && e_auth[0] != ACLERK){
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
                }else{
                    sprintf(sendBuf, "無効なコマンドです%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }
>>>>>>> Stashed changes
        }
    }
}

<<<<<<< Updated upstream
int service_kitchen(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
=======
void service_kitchen(int __soc, int *__auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
>>>>>>> Stashed changes
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;

<<<<<<< Updated upstream
    if(u_info[1] == AMGR || u_info[1] == ACLERK){
=======
    if(__auth[0] == AMGR || __auth[0] == ACLERK){
>>>>>>> Stashed changes
        while(1){
            sprintf(sendBuf, "完了した商品の登録は\"KFLAG\"，キッチン端末を終了するには\"KDEL\"を入力してください%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            recvLen = receive_message(soc, recvBuf, BUFSIZE); 
            if(recvLen != 0){
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, KFLAG) == 0){
		            if(kitchenFlag() != -1){
                        return 0;
                    }
                }else if(strcmp(comm, KDEL) == 0){
		            if(kitchenDel() != -1){ //=======キッチン終了関数kitchenDel
                        return 0;
                    }
                }else{
                    sprintf(sendBuf, "無効なコマンドです%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }   
        }
    }
}