#include "omos.h"

#define FGUEST       1
#define FEMPLOYEE    2

int service_table(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int tel, cnt;
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;

    if(u_info[1] == AMGR || u_info[1] == ACLERK){
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
                    tableDel(selfId, con, soc, recvBuf, sendBuf, s_info);
                    //flag = FEMPLOYEE;
                    //sprintf(sendBuf, "社員としてログインします%s", ENTER);
                    //send(soc, sendBuf, sendLen, 0);
                    //printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }else if(strcmp(comm, NO) == 0){
                    order(selfId, con, soc, recvBuf, sendBuf, u_info, s_info);
                    //flag = FGUEST;
                    //sprintf(sendBuf, "お客様としてログインします%s", ENTER);
                    //send(soc, sendBuf, sendLen, 0);
                    //printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }
            }
        }
    }else{
        order(selfId, con, soc, recvBuf, sendBuf, u_info, s_info);
    }

    //if(flag == FEMPLOYEE){
        //tableDel(selfId, con, soc, recvBuf, sendBuf, s_info);
    //}else{
       // order(selfId, con, soc, recvBuf, sendBuf, u_info, s_info);
    //}
    return 0;
}