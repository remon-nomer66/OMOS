#include "omos.h"

int userCheckSQL(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, char *tel, char *pass, int *u_info){
    int recvLen, sendLen;
    char sql[BUFSIZE];
    PGresult *res;
    int resultRows;

    //user_id，pass確認
    sprintf(sql, "SELECT * FROM user_t WHERE user_phone = %s AND user_pass = '%s'", tel, pass);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "データベースエラー%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows == 0){
        sprintf(sendBuf, "ユーザIDもしくはパスワードに誤りがあります%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }
    u_info[0] = atoi(PQgetvalue(res, 0, 0));  //user_id

    //returnされていないということは合致するユーザが存在
    sprintf(sql, "SELECT * FROM user_authority_t WHERE user_id = %d", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("%s", PQresultErrorMessage(res));
        sprintf(sendBuf, "データベースエラー%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }
    resultRows = PQntuples(res);
    if(resultRows != 1){
        sprintf(sendBuf, "データベースの対応関係に誤りがあります%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        return -1;
    }

    //ここまで来たということは，権限情報が正常に受け取れたということ
    u_info[1] = atoi(PQgetvalue(res, 0, 1));  //auth
    u_info[2] = atoi(PQgetvalue(res, 0, 2));  //store_id

    return 0;
}