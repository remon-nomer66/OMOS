#include "omos.h"

#define REWIND 1

int userCheck(PGconn *__soc){
    int gest_y_n = 0;   //ゲストかゲストでないか
    int tel;    //電話番号
    char comm[BUFSIZE];  //コマンド
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int auth, cnt;
    pthread_t selfId = pthread_self();

    while(1){
        //電話番号チェック
        while(1){
            sprintf(sendBuf, "会員の方は電話番号を入力してください．%s会員ではなく会員登録をされる方は\"UREG\"，ゲストでご利用される方は\"GEST\"と入力してください%s", ENTER, ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
            if(recvLen > 0){
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                cnt = sscanf(recvBuf, "%d", tel);
                if(cnt == 0){
                    if(strcmp(comm, UREG) == 0){
                        userReg();  //会員登録関数
                        sprintf(sendBuf, "会員の方は電話番号を入力してください．%s会員ではなく会員登録をされる方は\"UREG\"，ゲストでご利用される方は\"GEST\"と入力してください%s", ENTER, ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }else if(strcmp(comm, GUEST) == 0){
                        sprintf(sendBuf, "ゲストとしてログインします%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        gest_y_n = 1;
                        break;
                    }
                }else if(recvLen != TELLEN){
                    sprintf(sendBuf, "IDまたはコマンドを再度入力してください%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }else{
                    sprintf(sendBuf, "電話番号: 0%d%s", tel, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }
            }else{
                sprintf(sendBuf, "IDまたはコマンドを再度入力してください%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
        //パスワードチェック(ゲストなら以下は行わない)
        if(gest_y_n != 1){
            while(1){
                sprintf(sendBuf, "パスワードを入力してください%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                    sscanf(recvBuf, "%s", comm_1);
                    if((auth = userCheck()) != -1){   //戻り値-1でなければなら会員として正しい
                        break;
                    }else{
                        if(count == CEKMAX){
                            sprintf(sendBuf, "パスワード試行回数の上限に達しましたのでID入力画面に戻ります．%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            gest_y_n = 0;
                            count = 0;
                            break;
                        }else{
                            sprintf(sendBuf, "IDまたはパスワード，もしくはその両方が間違っています%sIDの入力からやり直しますか？IDの入力からやり直す場合は\"YES\"，パスワードを再入力する場合は\"NO\"を入力してください．%s", ENTER, ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE); 
                            if(recvLen != 0){
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                sscanf(recvBuf, "%s", comm_1);
                                if(strcmp(comm_1, YES) == 0){
                                    flag = REWIND;
                                }else if(strcmp(comm_1, NO) == 0){
                                    sprintf(sendBuf, "パスワードを再入力してください%s", ENTER);
                                    sendLen = strlen(sendBuf);
                                    send(__soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                    count++;
                                }else{
                                    sprintf(sendBuf, "不正なコマンドです%s", ENTER);
                                    sendLen = strlen(sendBuf);
                                    send(__soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                    flag = REWIND;
                                }
                            }else{
                                sprintf(sendBuf, "不正なコマンドです%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                flag = REWIND;
                            }
                        }
                        if(flag == REWIND){
                            break;
                        }
                    }
                }
            }
        }
        if(auth != -1){
            break;
        }
    }

    return auth;
}