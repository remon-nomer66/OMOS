#include "omos.h"

#define REWIND 1

//ユーザー認証
<<<<<<< Updated upstream
void service_user(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
=======
void service_user(PGconn *__con, int __soc, int *__u_info, int *__s_info){
>>>>>>> Stashed changes
    int gest_y_n = 0;   //ゲストかゲストでないか
    int tel;    //電話番号
    char comm[BUFSIZE];  //コマンド
    int recvLen, sendLen;
    int cnt, p_count, flag, a_flag;
<<<<<<< Updated upstream
    char buf[BUFSIZE];
    int i;
=======
   /*  int auth[2]; */
    pthread_t selfId = pthread_self();
>>>>>>> Stashed changes
    
    a_flag = 0;
    
    while(1){
        //電話番号チェック
        while(1){
<<<<<<< Updated upstream
            sprintf(sendBuf, "登録した電話番号を入力してください%s未登録の方で登録する場合は\"UREG\"，ゲストとしてログインする場合は\"GUEST\"を入力してください．%s%s", ENTER, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            recvLen = receive_message(soc, recvBuf, BUFSIZE);
=======
            
            recvLen = receive_message(__soc, recvBuf, BUFSIZE);
>>>>>>> Stashed changes
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
                        sprintf(sendBuf, "ゲストとしてログインします%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        gest_y_n = 1;
<<<<<<< Updated upstream
                        u_info[1] = 1;  //権限: お客様
=======
                        __auth[0] = 1;  //権限: お客様
>>>>>>> Stashed changes
                        break;
                    }
                }else if(strlen(tel) != TELLEN){
                    sprintf(sendBuf, "IDまたはコマンドを再度入力してください%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }else{
                    sprintf(sendBuf, "入力された携帯電話番号: 0%d%s", tel, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }
            }else{
                sprintf(sendBuf, "IDまたはコマンドを再度入力してください%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }

        //パスワードチェック(ゲストなら以下は行わない)
        if(gest_y_n != 1){
            i = 0;
            while(1){
                if(i = 0){
                    sprintf(sendBuf, "パスワードを入力してください%s%s", ENTER, DATA_END);
                }else{
                    sprintf(buf, "パスワードを入力してください%s%s", ENTER, DATA_END);
                    strcat(sendBuf, buf);
                }
                i++;
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                recvLen = receive_message(soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                    sscanf(recvBuf, "%s", comm);
<<<<<<< Updated upstream
                    if(getUserInfo(selfId, con, soc, tel, comm, u_info) != -1){ //戻り値-1でなければ会員として正しい
=======
                    if(getUserInfo(__con, __soc, tel, comm, __u_info, __s_info) != -1){   //戻り値-1でなければ会員として正しい
		                printf("userCheck\n;");
>>>>>>> Stashed changes
                        a_flag = 1;
		                break;
                    }else{
                        if(p_count == CEKMAX){
                            sprintf(sendBuf, "パスワード試行回数の上限に達しましたのでID入力画面に戻ります．%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            p_count = 0;
                            break;
                        }else{
                            sprintf(sendBuf, "IDまたはパスワード，もしくはその両方が間違っています%sIDの入力からやり直しますか？IDの入力からやり直す場合は\"YES\"，パスワードを再入力する場合は\"NO\"を入力してください．%s%s", ENTER, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            recvLen = receive_message(soc, recvBuf, BUFSIZE); 
                            if(recvLen > 0){
                                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                sscanf(recvBuf, "%s", comm);
                                if(strcmp(comm, YES) == 0){
                                    flag = REWIND;
                                }else if(strcmp(comm, NO) == 0){
                                    sprintf(sendBuf, "パスワードを再入力してください%s", ENTER);
                                    p_count++;
                                }else{
                                    sprintf(sendBuf, "不正なコマンドです%s", ENTER);
<<<<<<< Updated upstream
                                }
                            }else{
                                sprintf(sendBuf, "不正なコマンドです%s", ENTER);
=======
                                    sendLen = strlen(sendBuf);
                                    send(__soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                }
                            }else{
                                sprintf(sendBuf, "不正なコマンドです%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
>>>>>>> Stashed changes
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