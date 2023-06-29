#include "omos.h"

#define REWIND 1

//ユーザー認証
void service_user(PGconn *__con, int __soc, int *__auth){
    int gest_y_n = 0;   //ゲストかゲストでないか
    int tel;    //電話番号
    char comm[BUFSIZE];  //コマンド
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int cnt, p_count, flag, a_flag;
   /*  int auth[2]; */
    pthread_t selfId = pthread_self();

    while(1){
        //電話番号チェック
        while(1){
            a_flag = 0;
            sprintf(sendBuf, "会員の方は電話番号を入力してください．%s会員ではなく会員登録をされる方は\"UREG\"，ゲストでご利用される方は\"GUEST\"と入力してください%s", ENTER, ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(__soc, recvBuf, BUFSIZE);
            if(recvLen > 0){
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                cnt = sscanf(recvBuf, "%d", &tel);
                if(cnt == 0){
                    if(strcmp(comm, UREG) == 0){
		                if(userReg() == -1){
                            printf("error occured\n");
                        }  //会員登録関数
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
                    sprintf(sendBuf, "携帯電話番号%s0%d", ENTER, tel);
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
                recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                    sscanf(recvBuf, "%s", comm);
                    if(userCheck(__con, __soc, tel, comm) != -1){   //戻り値-1でなければ会員として正しい
		                printf("userCheck\n;");
                        a_flag = 1;
		                break;
                    }else{
                        if(p_count == CEKMAX){
                            sprintf(sendBuf, "パスワード試行回数の上限に達しましたのでID入力画面に戻ります．%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            gest_y_n = 0;
                            p_count = 0;
                            break;
                        }else{
                            sprintf(sendBuf, "IDまたはパスワード，もしくはその両方が間違っています%sIDの入力からやり直しますか？IDの入力からやり直す場合は\"YES\"，パスワードを再入力する場合は\"NO\"を入力してください．%s", ENTER, ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = receive_message(__soc, recvBuf, BUFSIZE); 
                            if(recvLen != 0){
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                sscanf(recvBuf, "%s", comm);
                                if(strcmp(comm, YES) == 0){
                                    flag = REWIND;
                                }else if(strcmp(comm, NO) == 0){
                                    sprintf(sendBuf, "パスワードを再入力してください%s", ENTER);
                                    sendLen = strlen(sendBuf);
                                    send(__soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                    p_count++;
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
        if(a_flag != 0){
            break;
        }
    }
}
