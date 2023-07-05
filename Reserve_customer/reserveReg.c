#include "omos.h"

int reserveReg(PGconn *__con, int __soc, int *__auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    char r_date[BUFSIZE], r_time[BUFSIZE];
    int p_num;
    int flag = 0;
    int cnt, i;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, r_flag;

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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows >= RSRVMAX){
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

    }
    PQclear(res);
    return 0;
}