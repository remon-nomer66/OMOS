#include "omos.h"

int pointCheck(PGconn *__con, int __soc, int *__u_info, pthread_t __selfId){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    int point;  //ポイント数

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
    sprintf(sql, "SELECT user_point FROM user_point_t WHERE user_id = %d;", __u_info[0]);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT failed: %s", PQerrorMessage(__con));
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
    point = atoi(PQgetvalue(res, 0, 1)); //ポイント数

    //トランザクション終了
    res = PQexec(__con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    return point;
}