#include "omos_http.h"

int response_reserve_check_NG(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *u_info, int *body_size){
    PGresult *res;
    char sql[BUFSIZE];
    int resultRows, tmp, i;
    int reserve_num[RSRVMAX];
    char reserve_date[RSRVMAX][11];
    char reserve_time[RSRVMAX][9];
    int reserve_store_id[RSRVMAX];
    char reserve_store_name[RSRVMAX][30];
    int size;

    http_body[0] = '\0';

    sprintf(sql, "SET search_path to reserve");
    res = PQexec(con, sql);
    sprintf(sql, "SELECT * FROM reserve_t WHERE user_id = %d", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
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
        return -1;
    }

    for(i = 0; i < resultRows; i++){
        reserve_num[i] = atoi(PQgetvalue(res, i, 2));
        strcpy(reserve_date[i], PQgetvalue(res, i, 3));
        strcpy(reserve_time[i], PQgetvalue(res, i, 4));
        reserve_store_id[i] = atoi(PQgetvalue(res, i, 5));
    }

    sprintf(sql, "SET search_path to public");
    PQexec(con, sql);

    for(i = 0; i < tmp; i++){
        sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %d", reserve_store_id[i]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
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

    sprintf(http_body, "<h5>予約一覧<h5><table border=\"1\"><tr><th>店舗名</th><th>日付</th><th>時間</th><th>人数</th>");
    for(i = 0; i < tmp; i++){
        sprintf(http_body + strlen(http_body), "<tr><td>%s</td><td>%s</td><td>%s</td><td>%d</td></tr>", reserve_store_name[i], reserve_date[i], reserve_time[i], reserve_num[i]);
    }
    sprintf(http_body + strlen(http_body), "</table><h5>予約可能な数を超えています<h5>");

    size = strlen(http_body);
    *body_size = size;
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}
