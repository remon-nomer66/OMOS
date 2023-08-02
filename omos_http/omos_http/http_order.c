#include "omos_http.h"

int http_order(pthread_t selfId, PGconn *con, int soc, int *s_info, int *u_info, int layer, char command[][BUFSIZE], char param[][BUFSIZE], char *http_body, char trimmd_comm[][5]){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, tmp;
    int a_table[20] = {0}; //利用可能な卓番号
    char r_hour[3], r_min[3];
    char *buf;
    int i, max;
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    int storage_buf;

    max = atoi(param[0]);

    for(i = 1; i <= max; i++){
        strtok(command[i], "-");
        buf = strtok(NULL, "\0");
        strcpy(trimmd_comm[i], buf);
    }

    PQexec(con, "BEGIN TRANSACTION");

    for(i = 1; i <= max; i++){
        sprintf(sql, "INSERT INTO order_t (store_id, desk_num, menu_id, order_cnt, kitchen_flag, order_date, order_time, user_id) VALUES (%d, %d, %s, %s, %d, '%d-%d-%d', '%d:%d:%d', %d)", s_info[0], s_info[1], trimmd_comm[i], param[i], 0, local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, u_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("%s", PQresultErrorMessage(res));
            PQexec(con, "ROLLBACK");
            return -1;
        }
        printf("1\n");
        sprintf(sql, "SELECT storage FROM menu_storage_t WHERE menu_id = %s AND store_id = %d", trimmd_comm[i], s_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            PQexec(con, "ROLLBACK");
            return -1;
        }
        printf("2\n");
        resultRows = PQntuples(res);
        if(resultRows != 1){
            PQexec(con, "ROLLBACK");
            return -1;
        }
        printf("3\n");
        storage_buf = atoi(PQgetvalue(res, 0, 0));
        if(storage_buf - atoi(param[i]) < 0){
            PQexec(con, "ROLLBACK");
            return -1;
        }
        printf("5\n");
        sprintf(sql, "UPDATE menu_storage_t SET storage = %d WHERE menu_id = %s AND store_id = %d", storage_buf - atoi(param[i]), trimmd_comm[i], s_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("%s", PQresultErrorMessage(res));
            PQexec(con, "ROLLBACK");
            return -1;
        }
        printf("6\n");
    }

    PQexec(con, "COMMIT");

    return 0;
}
