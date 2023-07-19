#include "omos_http.h"

int http_reserve_reg(pthread_t selfId, PGconn *con, int soc, int *u_info, char command[][BUFSIZE], char param[][BUFSIZE], char *body_part){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows, tmp;
    int a_table[20] = {0}; //利用可能な卓番号
    char r_hour[3], r_min[3]
    char *buf;

    //コマンド確認
    if(strcmp(command[1], "storeId") != 0){
        return -1;
    }
    if(strcmp(command[2], "date") != 0){
        return -1;
    }
    if(strcmp(command[3], "time") != 0){
        return -1;
    }
    if(strcmp(command[4], "num") != 0){
        return -1;
    }

    //当該店舗の卓について，人数的に許容できる卓の番号を格納
    sprintf(sql, "SELECT * FROM store_table_t WHERE store_id = %s AND desk_max >= %s ORDER BY desk_max ASC", param[1], param[4]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        PQclear(res);
        return -1;
    }
    tmp = resultRows = PQntuples(res);
    if(resultRows <= 0){
        PQclear(res);
        return -1;
    }
    for(i = 0; i < resultRows; i++){
        a_table[i] = atoi(PQgetvalue(res, i, 1));
    }

    param[3][6] = '\0';
    buf = strtok(param[3], ":");
    strncpy(r_hour, buf, sizeof(r_hour) - 1);
    buf = strtok(param[3], "\0");
    strncpy(r_min, buf, sizeof(r_min) - 1);

    //当該時間前後に卓の空きがあるのか，あるなら予約テーブルに情報追加
    sprintf(sql, "BEGIN TRANSACTION");
    PQexec(con, sql);
    for(i = 0; i < tmp; i++){
        sprintf(sql, "SET search_path to reserve");
        PQexec(con, sql);
        sprintf(sql, "SELECT * FROM reserve_t WHERE store_id = %s AND desk_num = %d AND reserve_date = '%s' AND reserve_time BETWEEN '%d:%s:00' AND '%d:%s:00'", param[1], a_table[i], param[2], atoi(r_hour) - 1, r_min, atoi(r_hour) + 1, r_min);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sql, "SET search_path to public");
            PQexec(con, sql);
            sprintf(sql, "ROLLBACK");
            PQexec(con, sql);
            PQclear(res);
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
            sprintf(sql, "INSERT INTO reserve_t (reserve_no, user_id, people_num, reserve_date, reserve_time, store_id, desk_num) VALUES (nextval('reserve_seq'), %d, %s, '%s', '%s', %s, %d)", u_info[0], param[4], param[2], param[3], param[1], a_table[i]);
            res = PQexec(con, sql);
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sql, "SET search_path to public");
                PQexec(con, sql);
                sprintf(sql, "ROLLBACK");
                PQexec(con, sql);
                PQclear(res);
                return -1;
            }
            sprintf(sql, "SET search_path to public");
            PQexec(con, sql);
            sprintf(sql, "COMMIT");
            PQexec(con, sql);
            flag = 1;
            break;
        }
    }

    if(flag != 1){     
        PQclear(res);               
        return 1;
    }
    PQclear(res);
    return 0;
}