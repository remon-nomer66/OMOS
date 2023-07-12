#include "omos.h"

int dispoint(PGconn *__con, int __soc, int dispoint, int *__u_info, pthread_t __selfId){

    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];

    //トランザクション開始
    PGresult *res = PQexec(__con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //u_info[0]を元に、user_point_tテーブルからuser_pointを取得
    sprintf(sql, "UPDATE user_point_t SET user_point = user_point - %d WHERE user_id = %d;", dispoint, __u_info[0]);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("UPDATE failed: %s", PQerrorMessage(__con));
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(__con));
            PQclear(res);
            PQfinish(__con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //トランザクション終了
    res = PQexec(__con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    return 0;
}

