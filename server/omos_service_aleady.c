#include "omos.h"

#define FGUEST       1
#define FEMPLOYEE    2

int service_table(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int tel, cnt;
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;

    if(u_info[1] != AMGR && u_info[1] != ACLERK){
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
        if(tableDel() != -1){
            s_info[0] = 0;  //店舗番号初期化
            s_info[1] = 0;  //卓番号初期化
            s_info[2] = 0;  //キッチンか否かのフラグ(一応0にしておく)
            return 0;
        }else{
            return -1;
        }
    }else{
        if(order(user_id, soc, __auth, __table_num, __store_id) != -1){
            return 0;
        }else{
            return -1;
        }
    }
}

int service_kitchen(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;

    if(u_info[1] == AMGR || u_info[1] == ACLERK){
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