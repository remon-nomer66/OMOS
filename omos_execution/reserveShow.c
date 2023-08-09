#include "omos.h"

int reserveShow(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, tmp, i, sendLen;
    int reserve_num[RSRVMAX];
    char reserve_date[RSRVMAX][11];
    char reserve_time[RSRVMAX][6];
    int reserve_store_id[RSRVMAX];
    char reserve_store_name[RSRVMAX][30];

    sprintf(sql, "SET search_path to reserve");
    res = PQexec(con, sql);
    sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);
        PQclear(res);
        return -1;
    }
    tmp = resultRows = PQntuples(res);
    if(resultRows == 0){
        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);
        PQclear(res);
        sprintf(sendBuf, "%s %d%s現在ご予約はありません%s", OK_STAT, 4, ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return 0;
    }

    for(i = 0; i < resultRows; i++){
        reserve_num[i] = atoi(PQgetvalue(res, i, 2));
        strncpy(reserve_date[i], PQgetvalue(res, i, 3), 10);
        reserve_date[i][10] = '\0';
        strncpy(reserve_time[i], PQgetvalue(res, i, 4), 5);
        reserve_time[i][5] = '\0';
        reserve_store_id[i] = atoi(PQgetvalue(res, i, 5));
    }

    sprintf(sql, "SET search_path to public");
    PQexec(con, sql);

    for(i = 0; i < tmp; i++){
        sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %d", reserve_store_id[i]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            PQclear(res);
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
            PQclear(res);
            return -1;
        }
        strcpy(reserve_store_name[i], PQgetvalue(res, 0, 0));
    }

    sprintf(sendBuf, "%s %d%s", OK_STAT, tmp + 1, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    sprintf(sendBuf, "予約一覧%s\t     店舗名\t日付\t  時間  人数%s", ENTER, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    for(i = 0; i < tmp; i++){
        sprintf(sendBuf, "%25s %10s %s %3d%s", reserve_store_name[i], reserve_date[i], reserve_time[i], reserve_num[i], ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }
}