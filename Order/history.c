#include "OMOS.h"

int history(pthread_t selfId, PGconn *__con, int *__s_store){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長

    //トランザクションの開始
    PGresult *res = PQexec(__con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }


    //order_tのstore_id = s_store[0], desk_num=s_store[1]となる一致するレコードを取得する
    char sql[BUFSIZE];
    sprintf(sql, "SELECT * FROM order_t WHERE store_id = %d AND desk_num = %d", __s_store[0], __s_store[1]);
    PGresult *res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("No data retrieved\n");
        PQclear(res);
        exit(1);
    }

    //絞り込んだテーブルを表示する。order_tのkitchenflagが「NULL」になっているものを上の方に表示する
    for(int i = 0; i < PQntuples(res); i++){ //PQntuples()は取得したレコード数を返す
        if(PQgetisnull(res, i, 4) == 1){    // kitchenflagが「NULL」
            printf("%s %s %s %s %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3), "未");   //PQgetvalue()は取得したレコードのi行目のj列目の値を返す
        }else{
            printf("%s %s %s %s %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3), "済み");  //PQgetvalue()は取得したレコードのi行目のj列目の値を返す
        }
    }

    //order_tにmenu_idが存在する。そのmenu_idを用いて、menu_price_tのmenu_idをキーにmenu_priceを取得する。そしてテーブルを統合する。
    sprintf(sql, "SELECT * FROM order_t INNER JOIN menu_price_t ON order_t.menu_id = menu_price_t.menu_id WHERE order_t.store_id = %d AND order_t.desk_num = %d", __s_store[0], __s_store[1]);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("No data retrieved\n");
        PQclear(res);
        exit(1);
    }

    //menu_priceを統合した絞り込んだテーブルのkitchenflagを確認する。「済み」だった場合、menu_priceと統合した絞り込んだテーブルのmenu_cntを掛け算し、合計金額を算出する
    int totalAmount = 0;

    for(int i = 0; i < PQntuples(res); i++){
        if(PQgetisnull(res, i, 4) == 0){ // kitchenflagが"済み"
            // menu_priceの取得
            sprintf(sql, "SELECT menu_price FROM menu_t WHERE menu_id = %s", PQgetvalue(res, i, 1));
            PGresult *resPrice = PQexec(__con, sql);
            if(PQresultStatus(resPrice) != PGRES_TUPLES_OK){
                printf("No data retrieved\n");
                PQclear(resPrice);
                exit(1);
            }
            // menu_priceとmenu_cntを掛け算し、合計金額を計算
            int menuPrice = atoi(PQgetvalue(resPrice, 0, 0));
            int menuCnt = atoi(PQgetvalue(res, i, 3));
            totalAmount += menuPrice * menuCnt;

            PQclear(resPrice); //メモリ解放

        }
    }

    //顧客に合計金額を表示する
    sprintf(sendBuf, "合計金額: %d%s", totalAmount, ENTER);
    sendLen = strlen(sendBuf);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //トランザクションの終了
    res = PQexec(__con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    PQclear(res);   //メモリ解放
    
}