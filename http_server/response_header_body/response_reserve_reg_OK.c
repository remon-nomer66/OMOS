#include "omos_http.h"

int response_reserve_reg_OK(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *u_info, int *body_size, char param[][BUFSIZE]){
    PGresult *res;
    char sql[BUFSIZE];
    int resultRows;

    http_body[0] = '\0';

    sprintf(sql, "SELECT store_name FROM store_t WHERE store_id = %s", param[1]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows != 1){
        return -1;
    }
    sprintf(http_body, "<h6>以下の予約が行われました</h6><h6>店舗名：%s</h6><h6>日付：%s</h6><h6>時刻：%s</h6><h6>人数：%s</h6>", PQgetvalue(res, 0, 0), param[2], param[3], param[4]);

    body_size = strlen(http_body);
    
    sprintf(http_header, "Content-Length: %d\r\n", body_size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加
    sprintf(http_header + strlen(http_header), "Location: /index.html");

    return 0;
}