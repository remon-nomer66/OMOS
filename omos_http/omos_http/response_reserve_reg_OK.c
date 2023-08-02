#include "omos_http.h"

int response_reserve_reg_OK(pthread_t selfId, PGconn *con, int soc, char *http_header, char *http_body, int *u_info, int *body_size, char param[][BUFSIZE]){
    PGresult *res;
    char sql[BUFSIZE];
    int resultRows, size;
    char time[6];

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

    printf("check\n");

    strncpy(time, param[3], 5);
    sprintf(http_body, "<h5>以下の予約が行われました</h5><table border=\"1\"><tr><td>店舗名</td><td>%s</td></tr><tr><td>日付</td><td>%s</td></tr><tr><td>時刻</td><td>%s</td></tr><tr><td>人数</td><td>%s</td></tr></table><br><a href=\"./reserve.html\">OK</a>", PQgetvalue(res, 0, 0), param[2], param[3], param[4]);

    size = strlen(http_body);
    
    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}