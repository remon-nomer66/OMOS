#include "omos.h"

#define REWIND 1

int num_len(int num);

//ユーザー認証
void service_user(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char tel[BUFSIZE];    //電話番号
    char comm[BUFSIZE];  //コマンド
    int recvLen, sendLen;
    int cnt, p_count, flag, a_flag;
    char buf[BUFSIZE];
    int i;
    
    a_flag = 0;
    
    while(1){
        //電話番号チェック
        while(1){
            sprintf(sendBuf, "登録した電話番号を入力してください%s未登録の方で登録する場合は\"UREG\"，ゲストとしてログインする場合は\"GUEST\"を入力してください．%s%s", ENTER, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen > 0){
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                cnt = sscanf(recvBuf, "%s", comm);
		        printf("tel: %s\n", comm);
                if(cnt == 1){
                    if(strcmp(comm, UREG) == 0){
		      //if(userReg() == -1){
		      //printf("error occured\n");
		      //}  //会員登録関数
                    }else if(strcmp(comm, GUEST) == 0){
                        sprintf(sendBuf, "ゲストとしてログインします%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
			            u_info[0] = 0;
                        u_info[1] = 1;  //権限: お客様
			            u_info[2] = 0;
                        return;
                    }else if(strlen(comm) != TELLEN){
                        sprintf(sendBuf, "%s %d%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }else{
                        strcpy(tel, comm);
                        sprintf(sendBuf, "入力された携帯電話番号: %s%s", tel, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        break;
                    }
                }else{
                    sprintf(sendBuf, "IDまたはコマンドを再度入力してください%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else{
                sprintf(sendBuf, "IDまたはコマンドを再度入力してください%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }

        //パスワードチェック(ゲストなら以下は行わない)
            while(1){
                sprintf(sendBuf, "パスワードを入力してください%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                recvLen = receive_message(soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                    sscanf(recvBuf, "%s", comm);
                    if(userCheckSQL(selfId, con, soc, recvBuf, sendBuf, tel + 1, comm, u_info) != -1){ //戻り値-1でなければ会員として正しい
                        sprintf(sendBuf, "処理完了%s%s", ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        a_flag = 1;
			            break;
                    }else{
                        if(p_count == CEKMAX){
                            sprintf(sendBuf, "パスワード試行回数の上限に達しましたのでID入力画面に戻ります%s", ENTER);
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
                                }
                            }else{
                                sprintf(sendBuf, "不正なコマンドです%s", ENTER);
                            }
                        }
                        if(flag == REWIND){
                            break;
                        }
                    }
                }
            }
        if(a_flag != 0){
            break;
        }
    }
}