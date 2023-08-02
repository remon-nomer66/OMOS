#include "omos_http.h"

int response_order_OK(pthread_t selfId, PGconn *con, int soc, char param[][BUFSIZE], char *http_header, char *http_body, int *u_info, int *body_size, char trimmd_comm[][5]){
    char sql[BUFSIZE];
    PGresult *res;
    int size;
    char *buf;
    int resultRows, i, max;
    
    http_body[0] = '\0';

    sprintf(http_body, "<p>注文が正常に完了しました</p><p>注文頂いた商品は以下のとおりです</p><table border=\"1\"><thead><tr><th>商品名</th><th>数量</th></tr></thead><tbody>");

    max = atoi(param[0]);

    for(i = 1; i <= max; i++){
        sprintf(sql, "SELECT * FROM recipe_t WHERE menu_id = %s", trimmd_comm[i]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
            return -1;
        }
        sprintf(http_body + strlen(http_body), "<tr><td>%s</td><td>%s</td></tr>", PQgetvalue(res, 0, 1), param[i]);
    }
    sprintf(http_body + strlen(http_body), "</tbody></table>");

    size = strlen(http_body);
    *body_size = size;

    sprintf(http_header, "Content-Length: %d\r\n", size);
    sprintf(http_header + strlen(http_header), "Access-Control-Allow-Origin: *\r\n"); // CORS回避のためのヘッダフィールドを追加

    return 0;
}