#include "omos.h"
#include "pay.h"

int pay(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf)
{
    int recvLen, sendLen;                    // 送受信データ長
    int i=0, num, money,change;
    char query[BUFSIZE];
    char sql[BUFSIZE];                       // SQL文
    int resultRows; // SELECTの結果の行数
    int evalue = 0; //評価

    // トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        printf("config(1)\n");
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_100, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    // Get the store number from u_info[2].
    int store_num = u_info[2];
    printf("store_num = %d\n", store_num);

    // お会計にてテーブル番号を入力してもらう
    sprintf(sendBuf, "テーブル番号を入力してください。%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    recvLen = receive_message(soc, recvBuf, BUFSIZE);
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

    // テーブル番号の取得
    int tableNum = atoi(recvBuf);
    printf("tableNum = %d\n", tableNum);

    // Check if there is a store_table_t table whose store_id matches store_num and desk_num matches tableNum.
    sprintf(sendBuf, "SELECT * FROM store_table_t WHERE store_id = %d AND desk_num = %d;", store_num, tableNum);
    res = PQexec(con, sendBuf);
    printf("sendBuf = %s\n", sendBuf);
    resultRows = PQntuples(res);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");

        PQclear(res);
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return -1;
    }

    // テーブル番号が存在しない場合
    if (resultRows == 0){
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2301, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        PQclear(res);
        return -1;
    }

    // テーブル番号が存在する場合
    // order_tよりテーブル番号(tableNum = desk_num)の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        sprintf(sendBuf, "%s%d%s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return -1;
    }

    // 注文が存在しない場合
    if (PQntuples(res) == 0)
    {
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2302, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        PQclear(res);
        return -1;
    }

    // 注文が存在する場合
    // order_tよりkitchen_flagが0の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 0;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");

        PQclear(res);
        sprintf(sendBuf, "%s%d %s", ER_STAT, E_CODE_100, ENTER);
        send(soc, sendBuf, sendLen, 0);
        return -1;
    }

    // kitchen_flagが0の注文が存在する場合
    if (PQntuples(res) != 0)
    {
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2303, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        PQclear(res);
        return -1;
    }

    // order_tよりkitchen_flagが1の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");

        PQclear(res);
        sprintf(sendBuf, "%s%d%s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return -1;
    }

    // kitchen_flagが1の注文が存在する場合、割り勘の有無を問う
    if (PQntuples(res) != 0)
    {
        sprintf(sendBuf, "割り勘しますか？(y/n)%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
        recvBuf[recvLen-1] = '\0'; //受信データを文字列にする

        // 割り勘する場合
        if (recvBuf[0] == 'y')
        {
            // order_tよりkitchen_flagが1の注文を取得する
            sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
            res = PQexec(con, sendBuf);
            if (PQresultStatus(res) != PGRES_TUPLES_OK)
            { // SELECT失敗
                printf("SELECT failed: %s", PQerrorMessage(con));
                // ロールバック
                res = PQexec(con, "ROLLBACK");
                PQclear(res);
                sprintf(sendBuf, "%s%d%s", ER_STAT, E_CODE_100,ENTER);
                send(soc, sendBuf, sendLen, 0);
                return 0;
            }

            // 割り勘する人数を入力してもらう
            sprintf(sendBuf, "割り勘する人数を入力してください。%s%s", ENTER,DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
            recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

            // 割り勘する人数の取得
            num = atoi(recvBuf);

            // 割り勘する人数が0以下の場合、もう一度入力してもらう
            while (num <= 0)
            {
                sprintf(sendBuf, "%s%d 入力値が不正です。割り勘する人数を入力してください。%s%s", ER_STAT, E_CODE_2304, ENTER, DATA_END );
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
                recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

                // 割り勘する人数の取得
                num = atoi(recvBuf);
            }

            // 割り勘する人数の取得
            sprintf(sendBuf, "割り勘する人数は%d人です。%s", num, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }

        // 割り勘しない場合
        else if(recvBuf[0] == 'n')
        {
            int num = 1;
            sprintf(sendBuf, "%s 1 割り勘しません。%s", OK_STAT,ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }

        else{
            sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2304, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            return -1;
        }
    }

    // order_tよりkitchen_flagが1の注文を取得した後、order_tのmenu_idをもとにmenu_price_tからpriceを結合する
    sprintf(sendBuf, "SELECT order_t.menu_id, menu_price_t.price FROM order_t INNER JOIN menu_price_t ON order_t.menu_id = menu_price_t.menu_id WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return -1;
    }

    // 金額を計算する
    // order_tの個数(order_cnt)とmenu_price_tの値段(price)をかけて、合計金額を計算する
    int sum;
    for (int i = 0; i < PQntuples(res); i++)
    {
        sum += atoi(PQgetvalue(res, i, 0)) * atoi(PQgetvalue(res, i, 1));
    }

    printf("sum = %d\n", sum);


    // ポイントの使用を問う
    sprintf(sendBuf, "ポイントを使用しますか？(y/n)%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示


    // Pointを使用する場合
    if (strcmp(recvBuf, "y") == 0 || strcmp(recvBuf, "Y") == 0){
        sprintf(sendBuf, "ポイントを使用します%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示

        int point;  //ポイント数
        int usePoint;   //使用するポイント数

        //u_infoを元に、pointをデータベースから取得
        sprintf(sql, "SELECT user_point FROM user_point_t WHERE user_id = %d;", u_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("SELECT failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("ROLLBACK failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
                send(soc, sendBuf, sendLen, 0);
            }
            PQclear(res);
            sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
            send(soc, sendBuf, sendLen, 0);
        }
        //ポイントをpointに格納する
        point = atoi(PQgetvalue(res, 0, 0));
        printf("point = %d\n", point);

        //pointを使用
        while(1){
            sendLen = sprintf(sendBuf, "現在のポイント数は%dです。何ポイント使用しますか？%s%s", point, ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);  //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = recv(soc, recvBuf, BUFSIZE, 0);    //受信
            recvBuf[recvLen-1] = '\0';    //文字列化
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            usePoint = atoi(recvBuf);   //使用するポイント数
            if(usePoint > point){   //現在のポイント数が使用したいポイントを超えていた場合、エラーを返し際入力を行う
                sendLen = sprintf(sendBuf, "使用したいポイントが現在のポイント数を超えています。%s", ENTER);    //送信
                send(soc, sendBuf, sendLen, 0);  //送信
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else{
                break;  //現在のポイント数が使用したいポイントを超えていない場合、ループを抜ける
            }
        }

        //使用分のpointをdis_point()関数に送信、引数をcon,soc, point, u_info,selfIdを渡す
        //u_info[0]を元に、user_point_tテーブルからuser_pointを取得
        int dispoint = usePoint;
        sprintf(sql, "UPDATE user_point_t SET user_point = user_point - %d WHERE user_id = %d;", dispoint, u_info[0]);
        res = PQexec(con, sql);
        printf("[C_THREAD %ld] UPDATE user_point_t: %s\n", selfId, sql);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("UPDATE failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("ROLLBACK failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
                send(soc, sendBuf, sendLen, 0);
            }
            PQclear(res);
            sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
            send(soc, sendBuf, sendLen, 0);
        }

        //合計金額を計算
        sum -= usePoint;
    }

    // 割り勘しない場合
    else if(strcmp(recvBuf, "n") == 0 || strcmp(recvBuf, "N") == 0){
        sprintf(sendBuf, "%s 1 ポイントを使用しない。%s", OK_STAT,  ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    else{

        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2304, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        return -1;
    }

    if(num == 0){
        num = 1;
    }
    printf("sum = %d\n", sum);
    // 割り勘する人数で割る
    sum = sum / num;      // 合計金額を人数で割る
    int over = sum % num; // 割り切れない場合の余りを求める

    // もしoverが0の時、
    if (over == 0)
    {
        // 合計金額を送信する
        sprintf(sendBuf, "%s 合計金額は%d円です。%s", OK_STAT,sum, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    // もしover(あまり)が0でない場合、
    else
    {
        // ○人は△円、□人は▲円です。と送信する
        sprintf(sendBuf, "%s %d人は%d円、%d人は%d円です。%s", OK_STAT, num - over, sum, over, sum + 1, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    // お金を受け取り、おつり計算を行う。
    if (num == 1)
    {
        // お客様から頂戴した金額を入力してもらう
        sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
        recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

        // お客様から頂戴した金額を取得
        money = atoi(recvBuf);
        //If money exceeds 10000000, issue an amount error.
        if(money > 10000000){
            sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2305, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            return -1;
        }

        // お客様から頂戴した金額が合計金額よりも少ない場合、
        else if(money < sum)
        {
            // お客様から頂戴した金額が合計金額よりも少ないことを伝える
            sprintf(sendBuf, "%s %d。%s", ER_STAT, E_CODE_2306, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            return -1;
        }
        else{
            // おつりを計算する
            change = money - sum;

            // おつりを送信する
            sprintf(sendBuf, "%s 1 おつりは%d円です。お会計終了です。%s", OK_STAT, change, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }
    }

    else{
        //(num - over)回sum円の会計を行う、over回sum+1円の会計を行う
        for(i=0;i<num-over;i++){
            sprintf(sendBuf, "%s %d回目の会計,%d円です。%s", OK_STAT, i+1, sum, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示

            sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
            recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

            money = 0;

            // お客様から頂戴した金額を取得
            money = atoi(recvBuf);

            //If money exceeds 10000000, issue an amount error.
            if(money > 10000000){
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2305, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                break;
            }

            // お客様から頂戴した金額が合計金額よりも少ない場合、
            while (money < sum)
            {
                // お客様から頂戴した金額が合計金額よりも少ないことを伝える
                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_2309, ENTER,DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信

                // お客様から頂戴した金額を取得
                money = atoi(recvBuf);
            }

            // お客様から頂戴した金額が合計金額よりも多い場合、
            if (money >= sum)
            {
                // おつりを計算する
                change = money - sum;

                // おつりを送信する
                sprintf(sendBuf, "%s 1 おつりは%d円です。%s", OK_STAT, change, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            }

            // お会計終了を伝える
            sprintf(sendBuf, "%s 1 お会計終了です。%s", OK_STAT,ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }

        for(i=0;i<over;i++){
            sprintf(sendBuf, "%s %d回目の会計,%d円です。%s", OK_STAT,i+num-over+1, sum+1, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示

            sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
            recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

            money = 0;

            // お客様から頂戴した金額を取得
            money = atoi(recvBuf);
            //If money exceeds 10000000, issue an amount error.
            if(money > 10000000){
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_2305, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                return -1;
            }

            // お客様から頂戴した金額が合計金額よりも少ない場合、
            while (money < sum)
            {
                // お客様から頂戴した金額が合計金額よりも少ないことを伝える

                sprintf(sendBuf, "%s %d お客様から頂戴した金額が合計金額よりも少ないです。もう一度入力してください。%s%s", ER_STAT, E_CODE_2307, ENTER,DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信

                // お客様から頂戴した金額を取得
                money = atoi(recvBuf);
            }

            // お客様から頂戴した金額が合計金額よりも多い場合、
            if (money >= sum)
            {
                // おつりを計算する
                change = money - sum;

                // おつりを送信する
                sprintf(sendBuf, "%s おつりは%d円です。%s", OK_STAT, change, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            }

            // お会計終了を伝える
            sprintf(sendBuf, "%s 1 お会計終了です。%s", OK_STAT, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }
    }

    // お会計終了を伝える
    sprintf(sendBuf, "%s 1 ありがとうございました。評価に移ります。%s", OK_STAT, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示

    // order_tから該当するテーブルのデータを移行する
    sprintf(sendBuf, "INSERT INTO summary_t SELECT store_id, menu_id, order_cnt, order_date, order_time, user_id FROM order_t WHERE desk_num = %d AND store_id = %d; ", tableNum, store_num);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("%s",PQresultErrorMessage(res));
        printf("error\n");
        PQclear(res);
        res = PQexec(con, "ROLLBACK");
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
        PQclear(res);
        return -1;

    }
    PQclear(res);

    printf("order_tからsummary_tへの移行完了\n");

    //order_tから該当する店舗番号、テーブルのデータを削除する
    //sprintf(sendBuf, "DELETE FROM order_t WHERE store_id = %d AND desk_num = %d;", store_num, tableNum);
    //res = PQexec(con, sendBuf);
    //if (PQresultStatus(res) != PGRES_COMMAND_OK){
    //    PQclear(res);
    //    res = PQexec(con, "ROLLBACK");
    //    sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
    //    PQclear(res);
    //    return -1;
    //}
    //PQclear(res);

    printf("order_tからの削除完了\n");

    // お客様の評価を行う,評価を行う関数を呼び出す
    while(1){
        sprintf(sendBuf, "お客様の評価を良いか悪いか,y,nで聞きます。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);  //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        recvBuf[recvLen - 1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        if(strcmp(recvBuf, "y") == 0){
            printf("y\n");
            evalue = 1;
            break;
        }else if(strcmp(recvBuf, "n") == 0){
            printf("n\n");
            evalue = 0;
            break;
        }else{
            sprintf(sendBuf, "yかnで入力してください。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);  //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
    }//END while()

    //user_point_tのuser_id=u_info[0]でSELECTする。うまくいかなかった場合、ロールバックする
    sprintf(sql, "SELECT user_mag FROM user_point_t WHERE user_id = %d;", u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100,ENTER);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    }

    //user_point_tのポイント倍率を取得
    float point_rate = atof(PQgetvalue(res, 0, 0));
    printf("%f\n",point_rate);

    //取得したポイント倍率を表示
    sendLen = sprintf(sendBuf, "ポイント倍率は%.1fです。%s", point_rate, ENTER);
    send(soc, sendBuf, sendLen, 0);  //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //良い場合は、ポイント倍率を+0.1する、悪い場合は-0.1する
    if(evalue == 1){
        printf("evalue == 1\n");
        if(point_rate >= 2.0){
            printf("point_rate > 2.0\n");
            point_rate = 2.0;
            printf("%f\n", point_rate);
        }else{
            printf("point_rate < 2.0\n");
            point_rate = point_rate + 0.1;
            printf("%f\n",point_rate);
        }
    }else{
        printf("evalue == 0\n");
        if(point_rate <= 0.2){
            printf("point_rate < 0.1\n");
            point_rate = 0.1;
            printf("%f\n",point_rate);
        }else{
            printf("point_rate > 0.1\n");
            point_rate = point_rate - 0.1;
            printf("%f\n",point_rate);
        }
    }

    //user_point_tのポイント倍率(user_mag)をpoint_rateに更新、うまくいかなかった場合、ロールバックする
    sprintf(sql, "UPDATE user_point_t SET user_mag = %f WHERE user_id = %d;", point_rate, u_info[0]);
    res = PQexec(con, sql);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("UPDATE failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //評価を送信
    sendLen = sprintf(sendBuf, "ポイント倍率[%.1f]評価を送信しました。%s", point_rate, ENTER);
    send(soc, sendBuf, sendLen, 0);  //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    
    sprintf(sendBuf, "%s 1 評価ありがとうございました。%s", OK_STAT, ENTER);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    // トランザクションの終了
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;
}