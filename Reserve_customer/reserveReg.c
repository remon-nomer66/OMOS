#include "omos.h"

<<<<<<< Updated upstream
int reserveReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen;   //送受信データ長
=======
int reserveReg(PGconn *__con, int __soc, int *__auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
>>>>>>> Stashed changes
    char r_date[BUFSIZE], r_time[BUFSIZE];
    int p_num;
    int flag = 0;
    int cnt, i;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, r_flag;
<<<<<<< Updated upstream
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
            sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

	        sprintf(sql, "SET search_path to public");
	        PQexec(con, sql);
=======

    while(1){
        //予約可能な回数を超えているかチェック
        sprintf(sql, "SELECT * FROM reserve_t WHERE account_id = %d", __auth[1]);
        res = PQexec(__con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "データベースエラー%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
>>>>>>> Stashed changes
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows >= RSRVMAX){
<<<<<<< Updated upstream
            sprintf(sendBuf, "予約可能な数を超えています%s%s", ENTER, DATA_END);
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
                sprintf(sendBuf, "ユーザ画面に戻ります%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }
        }
=======
            sprintf(sendBuf, "予約可能な数を超えています%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            return -1;
        }

        //予約可能な時
        r_flag = reserveCheck(__con, __soc, __auth);
        if(r_flag == 0){
            break;
        }

>>>>>>> Stashed changes
    }
    PQclear(res);
    return 0;
}