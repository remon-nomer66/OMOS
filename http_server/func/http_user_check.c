#include "omos_http.h"

int http_user_check(pthread_t selfId, PGconn *con, int soc, int *u_info, char command[][BUFSIZE], char param[][BUFSIZE]){
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows;

    //コマンド確認
    if(strcmp(command[1], "tel") != 0){
        return -1;
    }
    if(strcmp(command[2], "pw") != 0){
        return -1;
    }
    
    //user_id，pass確認
    sprintf(sql, "SELECT * FROM user_t WHERE user_phone = %s AND user_pass = '%s'", param[1], param[2]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows == 0){
        return -1;
    }
    u_info[0] = atoi(PQgetvalue(res, 0, 0));  //user_id

    //returnされていないということは合致するユーザが存在
    sprintf(sql, "SELECT * FROM user_authority_t WHERE user_id = %d", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows != 1){
        return -1;
    }

    //ここまで来たということは，権限情報が正常に受け取れたということ
    u_info[1] = atoi(PQgetvalue(res, 0, 1));  //auth
    u_info[2] = atoi(PQgetvalue(res, 0, 2));  //store_id

    return 0;
}