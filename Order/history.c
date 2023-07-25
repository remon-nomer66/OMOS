#include "omos.h"
#include "history.h"

int history(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    int num_price[200][4];  //menu_price_tのレコードを格納する配列
    int resultRows;

    //トランザクションの開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    printf("%d\n", s_info[0]);
    printf("%d\n", s_info[1]);


    //order_tのstore_id = s_store[0], desk_num=s_store[1]となる一致するレコードを取得する
    char sql[BUFSIZE];
    sprintf(sql, "SELECT menu_name, order_cnt, kitchen_flag FROM order_t INNER JOIN recipe_t ON order_t.menu_id = recipe_t.menu_id WHERE store_id = %d AND desk_num = %d;", s_info[0], s_info[1]);
    printf("%s\n", sql);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("No data retrieved\n");
        printf("%s\n", PQerrorMessage(con));
        PQclear(res);
        return -1;
    }
    resultRows = PQntuples(res);

    //結合したテーブルのkitche_flag=0のレコードを表示する。その際、kithen_flag の0を「未」と表示する
    for(int i = 0; i < resultRows; i++){
        if(PQgetisnull(res, i, 2) == 0){ // kitchenflagが"未"
            sprintf(sendBuf, "%35s %5s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), "未", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }
    //結合したテーブルのkitche_flag=1のレコードを表示する。その際、kithen_flag の1を「済」と表示する
    for(int i = 0; i < resultRows; i++){
        if(PQgetisnull(res, i, 2) == 1){ // kitchenflagが"済み"
            sprintf(sendBuf, "%35s %5s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), "済", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }

    //order_tにmenu_idが存在する。そのmenu_idを用いて、menu_price_tのmenu_idをキーにmenu_priceを取得する。そしてテーブルを統合する。
    sprintf(sql, "SELECT order_cnt, price, kitchen_flag FROM order_t INNER JOIN menu_price_t ON order_t.menu_id = menu_price_t.menu_id WHERE order_t.store_id = %d AND order_t.desk_num = %d;", s_info[0], s_info[1]);
    res = PQexec(con, sql);
    printf("%s\n", sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("No data retrieved\n");
        PQclear(res);
        exit(1);
    }
    resultRows = PQntuples(res);

    //金額を導出する。menu_price_tの結合されたテーブルのorder_cnt*price*kitchen_flagを行い、足し合わせて合計金額を算出する。
    int totalAmount = 0;
    for(int i = 0; i < resultRows; i++){
        if(PQgetisnull(res, i, 2) == 1){ // kitchenflagが"済み"
            totalAmount += atoi(PQgetvalue(res, i, 0)) * atoi(PQgetvalue(res, i, 1));
        }
    }
    
    printf("totalAmount: %d\n", totalAmount);

    //顧客に合計金額を表示する
    sprintf(sendBuf, "合計金額: %d%s", totalAmount, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //トランザクションの終了
    res = PQexec(con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    PQclear(res);   //メモリ解放

    return 0;
    
}