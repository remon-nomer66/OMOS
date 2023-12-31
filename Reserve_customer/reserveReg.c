#include "omos.h"
#include "reserve.h"

int reserveReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;   //送受信データ長
    char r_date[BUFSIZE], r_time[BUFSIZE];
    int p_num;
    int flag = 0;
    int cnt, i;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, r_flag;
    int reg_chg_flag = 0;   //reg: 0
    int reserve_no = 0;     //ダミーデータ
    char comm[BUFSIZE];

    while(1){
        //予約可能な回数を超えているかチェック
	    sprintf(sql, "SET search_path to reserve");
        res = PQexec(con, sql);
	    sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d", u_info[0]);
	    res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

	        sprintf(sql, "SET search_path to public");
	        PQexec(con, sql);
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows >= RSRVMAX){
            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1202, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

	        sprintf(sql, "SET search_path to public");
	        PQexec(con, sql);
            return -1;
        }
	    sprintf(sql, "SET search_path to public");
	    PQexec(con, sql);

        //予約可能な時
        reserveCheck(selfId, con, soc, recvBuf, sendBuf, u_info, reg_chg_flag, reserve_no);

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
                PQclear(res);
                return 0;
            }
        }
    }
    PQclear(res);
    return 0;
}