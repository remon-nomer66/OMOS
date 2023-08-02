#include "omos.h"
#include "reserve.h"

int reserveReg_s(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;   //送受信データ長
    char r_date[BUFSIZE], r_time[BUFSIZE];
    int p_num;
    int flag = 0;
    int cnt, i;
    char sql[BUFSIZE];
    int resultRows, r_flag;
    int reg_chg_flag = 0;   //reg: 0
    int reserve_no = 0;     //ダミーデータ
    char comm[BUFSIZE];

    while(1){
        reserveCheck_s(selfId, con, soc, recvBuf, sendBuf, u_info, reg_chg_flag, reserve_no);

	    sprintf(sendBuf, "予約の登録を続けますか？続ける場合は\"CONTE\"、終える場合は\"END\"と入力してください%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

	    recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

	        sscanf(recvBuf, "%s", comm);
	        if(strcmp(comm, END) == 0){
                break;
            }
        }
    }
    return 0;
}