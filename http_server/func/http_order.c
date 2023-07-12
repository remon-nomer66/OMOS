#include "omos_http.h"

int http_order(pthread_t selfId, PGconn *con, int soc, int *s_info, int layer, char command[][BUFSIZE], char param[][BUFSIZE], char *http_body){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, tmp;
    int a_table[20] = {0}; //利用可能な卓番号
    char r_hour[3], r_min[3]
    char *buf;
    int i, max;
    char trimmd_comm[5][5];

    max = atoi(param[0]);

    for(i = 1; i <= max; i++){
        strtok(command[i], "-");
        buf = strtok(NULL, "\0");
        strcpy(trimmd_comm[i], buf);
    }

    time_t t = time(NULL);
    struct tm *local = localtime(&t);

    sprintf(sql, "BEGIN TRANSACTION");
    PQexec(con, sql);
    for(i = 1; i <= max; i++){
        sprintf(sql, "INSERT INTO order_t (store_id, desk_num, menu_id, order_cnt, kitchen_flag, order_date, order_time, account_id) VALUES (%d, %d, %s, %s, %d, '%s-%s-%s', '%s:%s:%s', %d)", s_info[0], s_info[1], trimmd_comm[i], param[i], 0, local->tm_year, local->mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, u_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sql, "ROLLBACK");
            PQexec(con, sql);
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
            sprintf(sql, "ROLLBACK");
            PQexec(con, sql);
            return -1;
        }
    }
    sprintf(sql, "COMMIT");
    PQexec(con, sql);

    return 0;
}