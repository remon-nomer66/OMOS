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
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(__con));
            PQclear(res);
            PQfinish(__con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        return 0;
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
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(__con));
            PQclear(res);
            PQfinish(__con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        return 0;
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
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(__con));
            PQclear(res);
            PQfinish(__con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        return 0;
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
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(__con));
            PQclear(res);
            PQfinish(__con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        return 0;

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
                //ロールバック
                res = PQexec(__con, "ROLLBACK");
                if(PQresultStatus(res) != PGRES_COMMAND_OK){
                    printf("ROLLBACK failed: %s", PQerrorMessage(__con));
                    PQclear(res);
                    PQfinish(__con);
                    sprintf(sendBuf, "error occured%s", ENTER);
                    send(__soc, sendBuf, sendLen, 0);
                }
                PQclear(res);
                PQfinish(__con);
                sprintf(sendBuf, "error occured%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return 0;
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
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("ROLLBACK failed: %s", PQerrorMessage(__con));
            PQclear(res);
            PQfinish(__con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        return 0;
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
    else{
        //○人は△円、□人は▲円です。と送信する
        sprintf(sendBuf, "%d人は%d円、%d人は%d円です。%s", num - over, sum, over, sum + 1, ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
    }

    // お金を受け取り、おつり計算を行う。
    //もしnum=1の場合、
    if(num == 1){
        // お客様から頂戴した金額を入力してもらう
        sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

        // お客様から頂戴した金額を取得
        int money = atoi(recvBuf);

        // お客様から頂戴した金額が合計金額よりも少ない場合、
        while(money < sum){
            // お客様から頂戴した金額が合計金額よりも少ないことを伝える
            sprintf(sendBuf, "お客様から頂戴した金額が合計金額よりも少ないです。もう一度入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
            recvLen = receive_message(__soc, recvBuf, BUFSIZE); //受信

            // お客様から頂戴した金額を取得
            money = atoi(recvBuf);
        }

        // お客様から頂戴した金額が合計金額よりも多い場合、
        if(money >= sum){
            // おつりを計算する
            int change = money - sum;

            // おつりを送信する
            sprintf(sendBuf, "おつりは%d円です。%s", change, ENTER);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示
        }

        //お会計終了を伝える
        sprintf(sendBuf, "お会計終了です。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);  //送信データを表示

        //order_tから該当するテーブルのデータを移行する
        sprintf(query, "INSERT INTO summary_t SELECT store_id, menu_id, order_cnt, order_date, order_time, user_id FROM order_t WHERE tableNum = %d", desk_num);
        res = PQexec(__con, query);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            PQclear(res);
            res = PQexec(__con, "ROLLBACK");
            PQclear(res);
            PQfinish(__con);
            return 1;
        }
        PQclear(res);

        //order_tから該当するテーブルのデータを削除する
        sprintf(query, "DELETE FROM order_t WHERE tableNum = %d", desk_num);
        res = PQexec(__con, query);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            PQclear(res);
            res = PQexec(__con, "ROLLBACK");
            PQclear(res);
            PQfinish(__con);
            return 1;
        }
        PQclear(res);

        //お客様の評価を行う,評価を行う関数を呼び出す
        int flag = evalue(__soc, u_info);

        //flagが1の場合、エラーが発生したことを伝える
        if(flag == 1){
            sprintf(sendBuf, "error occured%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }
        //flagが0の場合、正常に評価が行われたことを伝える
        else{
            sprintf(sendBuf, "評価ありがとうございました。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
        }

        //トランザクションの終了
        res = PQexec(__con, "COMMIT");
        PQclear(res);
        PQfinish(__con);

        return 0;
    }
}
