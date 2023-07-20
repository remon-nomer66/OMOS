#include "omos.h"

int dispoint(pthread_t selfId, PGconn *con, int soc, int dispoint, int *u_info,char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char sql[BUFSIZE];

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //u_info[0]を元に、user_point_tテーブルからuser_pointを取得
    sprintf(sql, "UPDATE user_point_t SET user_point = user_point - %d WHERE user_id = %d;", dispoint, u_info[0]);
    res = PQexec(con, sql);
    printf("[C_THREAD %ld] UPDATE user_point_t: %s\n", selfId, sql);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("UPDATE failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    printf("[C_THREAD %ld] UPDATE user_point_t: OK\n", selfId);

    //トランザクション終了
    res = PQexec(con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    return 0;
}

