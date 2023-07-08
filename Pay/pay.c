#include "omos.h"

\* お会計処理 *\
int pay(PGconn *__con, int __soc, int *__u_info){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID

    //トランザクション開始
    PGresult *res = PQexec(__con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //お会計にてテーブル番号を入力してもらう
    sprintf(sendBuf, "テーブル番号を入力してください。%s", ENTER);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
    recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

    // テーブル番号の取得
    int tableNum = atoi(recvBuf);

    // テーブル番号が存在するか確認
    sprintf(sendBuf, "SELECT * FROM store_table_t WHERE desk_num = %d;", tableNum);
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){ //SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    // テーブル番号が存在しない場合
    if(PQntuples(res) == 0){
        sprintf(sendBuf, "テーブル番号が存在しません。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        PQclear(res);
        PQfinish(__con);
        return 0;
    }

    // テーブル番号が存在する場合
    // order_tよりテーブル番号(tableNum = desk_num)の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d;", tableNum);
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){ //SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    // 注文が存在しない場合
    if(PQntuples(res) == 0){
        sprintf(sendBuf, "注文が存在しません。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        PQclear(res);
        PQfinish(__con);
        return 0;
    }

    // 注文が存在する場合
    // order_tよりkitchen_flagが0の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 0;", tableNum);
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){ //SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }
    
    // kitchen_flagが0の注文が存在する場合
    if(PQntuples(res) != 0){
        sprintf(sendBuf, "まだ提供していない料理がございます。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        PQclear(res);
        PQfinish(__con);
        return 0;
    }

    // order_tよりkitchen_flagが1の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){ //SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    // kitchen_flagが1の注文が存在する場合、割り勘の有無を問う
    if(PQntuples(res) != 0){
        sprintf(sendBuf, "割り勘しますか？(y/n)%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

        // 割り勘する場合
        if(recvBuf[0] == 'y'){
            // order_tよりkitchen_flagが1の注文を取得する
            sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
            res = PQexec(__con, sendBuf);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){ //SELECT失敗
                printf("SELECT failed: %s", PQerrorMessage(__con));
                PQclear(res);
                PQfinish(__con);
                sprintf(sendBuf, "error occured%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
            }

            // 割り勘する人数を入力してもらう
            sprintf(sendBuf, "割り勘する人数を入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

            // 割り勘する人数の取得
            int num = atoi(recvBuf);

            // 割り勘する人数が0以下の場合、もう一度入力してもらう
            while(num <= 0){
                sprintf(sendBuf, "入力値が不正です。割り勘する人数を入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
                recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

                // 割り勘する人数の取得
                num = atoi(recvBuf);
            }

            // 割り勘する人数の取得
            sprintf(sendBuf, "割り勘する人数は%d人です。%s", num, ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        }

        // 割り勘しない場合
        else{
            num = 1;
            sprintf(sendBuf, "割り勘しません。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        }
    }
    
    // order_tよりkitchen_flagが1の注文を取得した後、order_tのmenu_idをもとにmenu_price_tからpriceを結合する
    sprintf(sendBuf, "SELECT order_t.menu_id, menu_price_t.price FROM order_t INNER JOIN menu_price_t ON order_t.menu_id = menu_price_t.menu_id WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){ //SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //金額を計算する
    //order_tの個数(order_cnt)とmenu_price_tの値段(price)をかけて、合計金額を計算する
    int sum = 0;
    for(int i = 0; i < PQntuples(res); i++){
        sum += atoi(PQgetvalue(res, i, 0)) * atoi(PQgetvalue(res, i, 1));
    }

    //ポイントの使用を問う
    sprintf(sendBuf, "ポイントを使用しますか？(y/n)%s", ENTER);
    send(__soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
    recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

    // ポイントを使用する場合、pointUse()にu_infoを引数として渡し、ポイントを使用する
    if(recvBuf[0] == 'y'){
        sum = pointUse(__soc, sum, u_info);
    }


    // 割り勘する人数で割る
    sum /= num; //合計金額を人数で割る
    over = sum % num;   //割り切れない場合の余りを求める

    //もしoverが0の時、
    if(over == 0){
        // 合計金額を送信する
        sprintf(sendBuf, "合計金額は%d円です。%s", sum, ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
    }

    // もしover(あまり)が0でない場合、
    if(over != 0){
        //○人は△円、□人は▲円です。と送信する
        sprintf(sendBuf, "%d人は%d円、%d人は%d円です。%s", num - over, sum, over, sum + 1, ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
    }

    // 









}
