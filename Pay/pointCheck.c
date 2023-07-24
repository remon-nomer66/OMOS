#include "omos.h"
#include "pay.h"

int pointCheck(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];  //SQL文
    int point;  //ポイント数

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //u_info[0]を元に、user_point_tテーブルからuser_pointを取得
    sprintf(sql, "SELECT user_point FROM user_point_t WHERE user_id = %d;", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2301, ENTER);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_2302, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }
    //ポイントをpointに格納する
    point = atoi(PQgetvalue(res, 0, 0));
    printf("point = %d\n", point);

    //トランザクション終了
    res = PQexec(con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    return point;
}