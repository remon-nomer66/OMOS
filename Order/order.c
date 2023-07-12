#include "omos.h"

<<<<<<< Updated upstream
int order(pthread_t selfId, PGconn *con, char *recvBuf, char *sendBuf, int soc, int *u_info, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    int layer = 1;     //メニューレベル(1は初期値)
    char sql[BUFSIZE], menu_buf[LONG_BUFSIZE], check_buf[BUFSIZE], buf[BUFSIZE];
=======
int order(PGconn *__con, int __soc, int *__auth, int __table_num, int __store_id){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int layer = 1;     //メニューレベル
    char sql[BUFSIZE], menu_buf[BUFSIZE], check_buf[BUFSIZE], buf[BUFSIZE];
>>>>>>> Stashed changes
    PGresult *res;
    int menu_info[100][2];  //[0]: menu_id，[1]: 押し
    int i, j, resultRows, menu_len, cnt;
    int param1, param2;
    int order_con[5][2];    //[0]: 商品番号，[1]: 個数
    int flag = 0;
<<<<<<< Updated upstream
    char comm[BUFSIZE];
    int e_auth[2];
=======
>>>>>>> Stashed changes

    while(1){

        //層番号から商品番号を獲得
        sprintf(sql, "SELECT * FROM push_t WHERE layer = %d", layer);
<<<<<<< Updated upstream
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
=======
        res = PQexec(__con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            sprintf(sendBuf, "データベースエラー%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
>>>>>>> Stashed changes
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
<<<<<<< Updated upstream
            sprintf(sendBuf, "このレイヤーにメニューは存在しません%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
=======
            sprintf(sendBuf, "このレイヤーにメニューは存在しません%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
>>>>>>> Stashed changes
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return -1;
        }
        menu_len = resultRows;
        for(i = 0; i < menu_len; i++){
            menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
            if((atoi(PQgetvalue(res, i, 1)) == 1) || (atoi(PQgetvalue(res, i, 2)) == 1) || (atoi(PQgetvalue(res, i, 3)) == 1)){
                menu_info[i][1] = 1;
            }
        }

        //商品番号から商品名，価格を受け取り表示
        if(layer == 1){
            sprintf(menu_buf, "現在ご覧のメニューは\"コモンメニュー\"です%s", ENTER);
        }else if(layer == 2){
            sprintf(menu_buf, "現在ご覧のメニューは\"ブランドメニュー\"です%s", ENTER);
        }else if(layer == 3){
            sprintf(menu_buf, "現在ご覧のメニューは\"ショップメニュー\"です%s", ENTER);
        }else if(layer == 4){
            sprintf(menu_buf, "現在ご覧のメニューは\"リージョナルメニュー\"です%s", ENTER);
        }else{
            sprintf(menu_buf, "現在ご覧のメニューは\"シーズンメニュー\"です%s", ENTER);
        }
        sprintf(buf, "利用可能なメニューは以下のとおりです%s商品番号　　　商品名　　　　　価格　　%s", ENTER, ENTER);
        strcat(menu_buf, buf);
        for(i = 0; i < menu_len; i++){
<<<<<<< Updated upstream
            sprintf(sql, "SELECT menu_price_t.menu_id, menu_price_t.price, recipe_t.menu_name FROM recipe_t, menu_price_t, menu_storage_t WHERE menu_id = %d AND menu_storage_t.store_id = %d", menu_info[i][0], s_info[0]);
            res = PQexec(con, sql);
=======
            sprintf(sql, "SELECT menu_price_t.menu_id, menu_price_t.price, recipe_t.menu_name,  FROM recipe_t, price, menu_storage_t WHERE menu_id = %d AND menu_storage_t.store_id = %d", menu_info[i][0], __store_id);
            res = PQexec(__con, sql);
>>>>>>> Stashed changes
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "データベースエラー%s", ENTER);
                sendLen = strlen(sendBuf);
<<<<<<< Updated upstream
                send(soc, sendBuf, sendLen, 0);
=======
                send(__soc, sendBuf, sendLen, 0);
>>>>>>> Stashed changes
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows != 1){
                sprintf(sendBuf, "データエラー%s", ENTER);
                sendLen = strlen(sendBuf);
<<<<<<< Updated upstream
                send(soc, sendBuf, sendLen, 0);
=======
                send(__soc, sendBuf, sendLen, 0);
>>>>>>> Stashed changes
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return -1;
            }
            sprintf(buf, "%5d %10s %7d", menu_info[i][0], PQgetvalue(res, 0, 2), atoi(PQgetvalue(res, 0, 1)));
            strcat(menu_buf, buf);
            if(menu_info[i][1] == 1){
                sprintf(buf, "　　*%s", ENTER);
                strcat(menu_buf, buf);
            }
        }
        sprintf(buf, "メニューを移動する際は以下の数字を入力してください%s", ENTER);
        strcat(menu_buf, buf);
        if(layer == 1){
            sprintf(buf, "\"ブランドメニュー\"：2%s\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s\"シーズンメニュー\"：5%s", ENTER, ENTER, ENTER, ENTER);
        }else if(layer == 2){
            sprintf(buf, "\"コモンメニュー\"：1%s\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s\"シーズンメニュー\"：5%s", ENTER, ENTER, ENTER, ENTER);
        }else if(layer == 3){
            sprintf(buf, "\"コモンメニュー\"：1\"ブランドメニュー\"：2%s\"リージョナルメニュー\"：4%s\"シーズンメニュー\"：5%s", ENTER, ENTER, ENTER, ENTER);
        }else if(layer == 4){
            sprintf(buf, "\"コモンメニュー\"：1\"ブランドメニュー\"：2%s\"ショップメニュー\"：3%s\"シーズンメニュー\"：5%s", ENTER, ENTER, ENTER, ENTER);
        }else{
            sprintf(buf, "\"コモンメニュー\"：1\"ブランドメニュー\"：2%s\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s", ENTER, ENTER, ENTER, ENTER);
        }
        strcat(menu_buf, buf);
<<<<<<< Updated upstream
        sprintf(menu_buf, "注文する際は\"(商品番号) (個数)\"と入力してください%s注文の種類数は5種類以下，個数は5個以下でご注文ください．%s商品及び個数の指定が全て終了しましたら，ENTERボタンを押してください%s履歴を確認する場合は\"HIST\"，会計を行う場合は\"PAY\"を入力してください%s%s", ENTER, ENTER, ENTER, ENTER, DATA_END);
        sendLen = strlen(menu_buf);
        send(soc, menu_buf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, menu_buf);

        //クライアントからのメッセージを解析
        i = 1;
        while(i <= 5){
            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen == 0){   //空白が入力されたと判断
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
=======
        sprintf(menu_buf, "注文する際は\"(商品番号) (個数)\"と入力してください%s注文の種類数は5種類以下，個数は5個以下でご注文ください．%s商品及び個数の指定が全て終了しましたら，ENTERボタンを押してください%s", ENTER, ENTER, ENTER);
        sendLen = strlen(menu_buf);
        send(__soc, menu_buf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, menu_buf);

        //クライアントからのメッセージを解析
        i = 0;
        while(i <= 5){
            recvLen = receive_message(__soc, recvBuf, BUFSIZE);
            if(recvLen == 0){   //空白が入力されたと判断
>>>>>>> Stashed changes
                break;
            }
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
<<<<<<< Updated upstream
            
            cnt = sscanf(recvBuf, "%d %d", param1, param2);
=======
            cnt = sscanf("%d %d", param1, param2);
>>>>>>> Stashed changes
            if(cnt == 1 && i == 0){
                if((1 <= param1) && (param1 <= 5)){
                    layer = param1;
                }
            }else if(cnt == 2 && param2 <= 5){
                order_con[i][0] = param1;   //商品番号
                order_con[i][1] = param2;   //個数
                i++;
<<<<<<< Updated upstream
            }else if(cnt == 2 && param2 > 5){
                sprintf(sendBuf, "商品の個数は5個以下で入力してください%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(cnt == 0){
                cnt = sscanf(recvBuf, "%s", comm);
                if(cnt == 1){
                    if(strcmp(comm, HIST)){
                        if(history() != -1){
                            return 0;
                        }else{
                            return -1;
                        }
                    }else if(strcmp(comm, PAY)){
                        if(pay() != -1){
                            return 0;
                        }else{
                            return -1;
                        }
                    }
                }
=======
            }else if(param2 > 5){
                sprintf(sendBuf, "商品の個数は5個以下で入力してください%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
>>>>>>> Stashed changes
            }
        }

        //入力内容確認
        if(i != 0){
            sprintf(check_buf, "入力された内容は以下のとおりです%s", ENTER);
            for(j = 0; j <= i; j++){
                sprinf(sql, "SELECT menu_name FROM recipe_t WHERE menu_id = %d", order_con[j][0]);
<<<<<<< Updated upstream
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
=======
                res = PQexec(__con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
>>>>>>> Stashed changes
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
<<<<<<< Updated upstream
                    sprintf(sendBuf, "商品番号(%d)が誤っています%s%s", order_con[j][0], ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
=======
                    sprintf(sendBuf, "FOLLOW%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                    sprintf(sendBuf, "商品番号(%d)が誤っています%s", order_con[j][0], ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
>>>>>>> Stashed changes
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }
                sprintf(buf, "%5d %10s%s", order_con[j][0], PQgetvalue(res, 0, 0), ENTER);
                strcat(check_buf, buf);
            }
            sprintf(buf, "入力された内容に問題が無ければENTERを入力してください%s", ENTER);
            strcat(check_buf, buf);
            sendLen = strlen(check_buf);
<<<<<<< Updated upstream
            send(soc, check_buf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, check_buf);
            flag = 1;
        }
        if(flag == 1){
            //ENTERが入力されたか
            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen == 0){
                sprintf(sendBuf, "注文画面に戻ります%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return 0;
            }

            //order_tに登録
            time_t t = time(NULL);
            struct tm *local = localtime(&t);
            sprintf(sql, "BEGIN TRANSACTION");
            PQexec(con, sql);
            for(j = 0; j <= i; j++){
                sprintf(sql, "INSERT INTO order_t (store_id, desk_num, menu_id, order_cnt, kitchen_flag, order_date, order_time, account_id) VALUES (%d, %d, %d, %d, %d, '%s-%s-%s', '%s:%s:%s', %d)", s_info[0], s_info[1], order_con[j][0], order_con[j][1], 0, local->tm_year, local->mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, u_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "データベースエラー%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    sprintf(sql, "ROLLBACK");
                    PQexec(con, sql);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
                    sprintf(sendBuf, "データベースエラー%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    sprintf(sql, "ROLLBACK");
                    PQexec(con, sql);
                    return -1;
                }
            }
            sprintf(sql, "COMMIT");
            PQexec(con, sql);

            sprintf(sendBuf, "注文が正常に完了しました%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
=======
            send(__soc, check_buf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, check_buf);
            flag = 1;
        }
        if(flag == 0){
            break;
        }

        //ENTERが入力されたか
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if(recvLen != 0){
            sprintf(sendBuf, "FOLLOW%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            sprintf(sendBuf, "注文画面に戻ります%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            break;
        }

        //order_tに登録
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        sprintf(sql, "BEGIN TRANSACTION");
        PQexec(__con, sql);
        for(j = 0; j <= i; j++){
            sprintf(sql, "INSERT INTO order_t (store_id, desk_num, menu_id, order_cnt, kitchen_flag, order_date, order_time, account_id) VALUES (%d, %d, %d, %d, %d, '%s-%s-%s', '%s:%s:%s', %d)", __store_id, __table_num, order_con[j][0], order_con[j][1], 0, local->tm_year, local->mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, __auth[1]);
            res = PQexec(__con, sql);
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("%s", PQresultErrorMessage(res));
                sprintf(sendBuf, "データベースエラー%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                sprintf(sql, "ROLLBACK");
                PQexec(__con, sql);
                return -1;
            }
            resultRows = PQntuples(res);
            if(resultRows != 1){
                sprintf(sendBuf, "データベースエラー%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                sprintf(sql, "ROLLBACK");
                PQexec(__con, sql);
                return -1;
            }
        }
        sprintf(sql, "COMMIT");
        PQexec(__con, sql);

        sprintf(sendBuf, "FOLLOW%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        sprintf(sendBuf, "注文が正常に完了しました%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
>>>>>>> Stashed changes
    }
    PQclear(res);
    return 0;
}