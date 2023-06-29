#include "OMOS.h"

int history(PGconn *__con, int get_table_num){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID

    //DBに接続する
    sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
    __con = PQconnectdb(connInfo);
    if(PQstatus(__con) == CONNECTION_BAD){
        printf("DB connection error.\n");
        exit(1);
    }

    //orderテーブルのtable_numがhistory関数の引数get_table_numと一致するレコードを取得する
    sprintf(sql, "SELECT * FROM order WHERE table_num = %d", get_table_num);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("No data retrieved\n");
        PQclear(res);
        PQfinish(__con);
        exit(1);
    }

    //絞り込んだテーブルを表示する。orderテーブルのkitchenflagが「NULL」になっているものを上の方に表示する
    for(int i = 0; i < PQntuples(res); i++){ //PQntuples()は取得したレコード数を返す
        if(PQgetisnull(res, i, 4) == 1){    // kitchenflagが「NULL」
            printf("%s %s %s %s %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3), "未");   //PQgetvalue()は取得したレコードのi行目のj列目の値を返す
        }else{
            printf("%s %s %s %s %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3), "済み");  //PQgetvalue()は取得したレコードのi行目のj列目の値を返す
        }
    }

    //絞り込んだテーブルにmenu_idが存在する。そのmenu_idを用いて、menuテーブルからmenu_priceを取得する。そしてテーブルを統合する。
    sprintf(sql, "SELECT order.*, menu.menu_price FROM order "
             "INNER JOIN menu ON order.menu_id = menu.menu_id "
             "WHERE table_num = %d", get_table_num);
    res = PQexec(__con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("No data retrieved\n");
        PQclear(res);
        PQfinish(__con);
        exit(1);
    }

    //menu_priceを統合した絞り込んだテーブルのkitchenflagを確認する。「済み」だった場合、menu_priceと統合した絞り込んだテーブルのmenu_cntを掛け算し、合計金額を算出する
    int totalAmount = 0;

    for(int i = 0; i < PQntuples(res); i++){
        if(PQgetisnull(res, i, 4) == 0){ // kitchenflagが"済み"
            // menu_priceの取得
            sprintf(sql, "SELECT menu_price FROM menu WHERE menu_id = %s", PQgetvalue(res, i, 1));
            PGresult *resPrice = PQexec(__con, sql);
            if(PQresultStatus(resPrice) != PGRES_TUPLES_OK){
                printf("No data retrieved\n");
                PQclear(resPrice);
                PQfinish(__con);
                exit(1);
            }
            // menu_priceとmenu_cntを掛け算し、合計金額を計算
            int menuPrice = atoi(PQgetvalue(resPrice, 0, 0));
            int menuCnt = atoi(PQgetvalue(res, i, 3));
            totalAmount += menuPrice * menuCnt;

            PQclear(resPrice); //メモリ解放

        }
    }

    printf(" 合計金額: %d\n", totalAmount);

    //DBとの接続を切断する
    PQclear(res);
    PQfinish(__con);

}