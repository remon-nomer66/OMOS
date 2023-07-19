#include "omos_http.h"

int http_reserve_check(pthread_t selfId, PGconn *con, int soc, int *u_info){
    char sql[BUFSIZE];
    int resultRows;

    //予約可能な回数を超えているかチェック
    sprintf(sql, "SET search_path to reserve");
    res = PQexec(con, sql);
    sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows >= RSRVMAX){
        sprintf(sql, "SET search_path to public");
        PQexec(con, sql);
        return 1;
    }
    sprintf(sql, "SET search_path to public");
    PQexec(con, sql);

    return 0;
}