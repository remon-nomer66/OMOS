#include "omos_http.h"

int http_reserve_store(pthread_t selfId, PGconn *con, int soc, char *http_body, int *body_size){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, size;

    sprintf(sql, "SELECT * FROM store_t WHERE store_id >= 10");
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        PQclear(res);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows == 0){
        return -1;
    }
    for(i = 0; i < resultRows; i++){
        sprintf(http_body + strlen(http_body), "<option value=\"%d\">%s</option>", atoi(PQgetvalue(res, i, 0)), PQgetvalue(res, i, 1));
    }

    size = strlen(http_body);
    *body_size = size;

    sprintf(http_header, "Content-Length: %d\r\n", body_size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}