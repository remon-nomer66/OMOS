#include "omos.h"

int demandReg(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    char comm[BUFSIZE];
    int recvLen, sendLen;
    int store_id;

    //トランジション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1801, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    //auth情報の取得
    int auth = u_info[1];

    //store_idを取得する
    store_id = u_info[2];

    if(auth == 4){
        //menu_storage_tのstore_idと同様のものかつ、menu_storage_tのstorage_flag == 1のものを表示する
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, menu_name FROM menu_storage_t INNER JOIN recipe_t ON menu_storage_t.menu_id = recipe_t.menu_id WHERE store_id = %d AND storage_flag = 1;", store_id);
        res = PQexec(con, sendBuf);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        //検索結果が0件の場合、発注中の商品はありませんと返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "%s %d %s%s", ER_STAT, E_CODE_1802, ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return 0;
        }

        //検索結果が1件以上の場合、発注中の商品を表示する
        if(PQntuples(res) >= 1){
            sprintf(sendBuf, "発注中の商品は以下の通りです。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            //検索結果を表示する
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s:%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }

        //発注物が届いた場合"1"、発注物が届いていない場合"0"を入力してもらう
        while(1){
            sprintf(sendBuf, "発注物が届いた場合は「1」、届いていない場合は「0」を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            if(recvLen > 0){
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);
                if(strcmp(comm, "0") == 0){
                    break;
                } else if(strcmp(comm, "1") == 0){
                    break;
                }else{
                    sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1803, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }
        }

        //入力してもらった値が"0"の場合、処理を終了する
        if(strcmp(comm, "0") == 0){
            sprintf(sendBuf, "処理を終了します。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return 0;
        }

        //入力してもらった値が文字列が"1"の場合、在庫補充したい商品番号を入力してもらう
        if(strcmp(comm, "1") == 0){
            while(1){
                sprintf(sendBuf, "届いた商品番号を入力してください。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                recvLen = receive_message(soc, recvBuf, BUFSIZE);
                recvBuf[recvLen - 1] = '\0';
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                sscanf(recvBuf, "%s", comm);

                //入力してもらった商品番号をmenu_storage_tから検索する
                sprintf(sendBuf, "SELECT * FROM menu_storage_t WHERE store_id = %d AND storage_flag = 1 AND menu_id = %s", store_id, comm);
                res = PQexec(con, sendBuf);
                //検索結果が1件の場合、while文を抜ける
                if(PQntuples(res) == 1){
                    //menu_storage_tのstorage_flagを0にする
                    sprintf(sendBuf, "UPDATE menu_storage_t SET storage_flag = 0 WHERE store_id = %d AND storage_flag = 1 AND menu_id = %s", store_id, comm);
                    res = PQexec(con, sendBuf);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    break;
                }else{
                    sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1804, ENTER);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }

            //対応する店舗IDと商品番号をstore_order_tから探し出し、store_order_cntを取得する
            sprintf(sendBuf, "SELECT store_order_cnt FROM store_order_t WHERE store_id = %d AND menu_id = %s", store_id, comm);
            res = PQexec(con, sendBuf);
            //store_order_cntを取得する
            char *store_order_cnt_str = PQgetvalue(res, 0, 0);
            int store_order_cnt = atoi(store_order_cnt_str);
            printf("%d\n", store_order_cnt);

            
            //menu_storage_tの在庫個数(storage)にstore_order_cntを足す
            sprintf(sendBuf, "UPDATE menu_storage_t SET storage = storage + %d WHERE store_id = %d AND menu_id = %s", store_order_cnt, store_id, comm);
            res = PQexec(con, sendBuf);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //error処理
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("UPDATE failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1805, ENTER);
                send(soc, sendBuf, sendLen, 0);
            }

            //store_order_tから削除する
            sprintf(sendBuf, "DELETE FROM store_order_t WHERE store_id = %d AND menu_id = %s", store_id, comm);
            res = PQexec(con, sendBuf);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //error処理
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("DELETE failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1806, ENTER);
                send(soc, sendBuf, sendLen, 0);
            }
        }
    }

    if(auth == 2){
        //発注要求のあったドリンクを表示する
        sprintf(sendBuf, "発注要求のあったドリンク一覧を表示します。%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        //発注ドリンクの検索
        sprintf(sendBuf,"SELECT * FROM menu_storage_t INNER JOIN recipe_t ON menu_storage_t.menu_id = recipe_t.menu_id WHERE storage_flag = 1 AND fod = 1;");
        res = PQexec(con, sendBuf);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        //Error処理
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("SELECT failed: %s", PQerrorMessage(con));
            PQclear(res);
            PQfinish(con);
            sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
            send(soc, sendBuf, sendLen, 0);
        }

        //検索結果が0件の場合、発注中の商品はありませんと返す
        if(PQntuples(res) == 0){
            sprintf(sendBuf, "発注中の商品はありません。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return 0;
        }

        //検索結果が1件以上の場合、発注中の商品を表示する
        if(PQntuples(res) >= 1){
            sprintf(sendBuf, "発注中の商品は以下の通りです。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            //検索結果を表示する
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s:%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 6), ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }

        //発注表を作成しますか？
        sprintf(sendBuf, "発注表を作成しますか？(y/n)%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        //y/nを入力してもらう
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        recvBuf[recvLen - 1] = '\0';
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        sscanf(recvBuf, "%s", comm);

        //入力してもらった値が"n"の場合、処理を終了する
        if(strcmp(comm, "n") == 0){
            sprintf(sendBuf, "処理を終了します。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            return 0;
        }

        //入力してもらった値が"y"の場合、発注表を作成する
        if(strcmp(comm, "y") == 0){

            sprintf(sendBuf, "SELECT user_name FROM user_t WHERE user_id = %d;", u_info[0]);
            res = PQexec(con, sendBuf);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            char *user_name = PQgetvalue(res, 0, 0);
            printf("%s\n", user_name);

            sprintf(sendBuf, "発注日を入力してください。(例20230606)`%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            //発注日を入力してもらう
            recvLen = receive_message(soc, recvBuf, BUFSIZE);
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

            //20230606を2023-06-06とdateに格納する
            char date[11];
            sprintf(date, "%c%c%c%c-%c%c-%c%c", recvBuf[0], recvBuf[1], recvBuf[2], recvBuf[3], recvBuf[4], recvBuf[5], recvBuf[6], recvBuf[7]);
            printf("%s\n", date);

            FILE *file = fopen("order.txt", "w");
            if (file == NULL) {
                printf("Could not open file\n");
                return 1;
            }
            char header[] = "--------------------発注表--------------------";
            int responsible = u_info[0];
            char menu_name[100];
            int store_id,menu_id,store_order_cnt;

            fprintf(file, "%s\n", header);
            fprintf(file, "%s\n", date);
            fprintf(file, "%s\n\n", user_name);

            fprintf(file, "|店舗ID(store_id)|商品ID(menu_id)|商品名(menu_name)|個数(store_order_cnt)|\n");
            sprintf(sendBuf, "SELECT store_order_t.store_id, store_order_t.menu_id, menu_name, store_order_cnt FROM store_order_t INNER JOIN recipe_t ON store_order_t.menu_id = recipe_t.menu_id INNER JOIN menu_storage_t ON store_order_t = menu_storage_t WHERE storage_flag = 1 AND fod = 1;");
            res = PQexec(con, sendBuf);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //Error処理
            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                printf("SELECT failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_100, ENTER);
                send(soc, sendBuf, sendLen, 0);
            }

            for(int i = 0; i < PQntuples(res); i++){
                store_id = atoi(PQgetvalue(res, i, 0));
                menu_id = atoi(PQgetvalue(res, i, 1));
                strcpy(menu_name, PQgetvalue(res, i, 2));
                store_order_cnt = atoi(PQgetvalue(res, i, 3));

                fprintf(file, "|%d|%d|%s|%d|\n", store_id, menu_id, menu_name, store_order_cnt);
            }

            fclose(file);

            sprintf(sendBuf, "発注表を作成しました。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            //store_order_tのstore_order_cntを0にする
            sprintf(sendBuf, "UPDATE menu_storage_t SET storage_flag = 0 FROM recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND recipe_t.fod = 1 AND menu_storage_t.storage_flag = 1;");
            res = PQexec(con, sendBuf);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            //error処理
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                printf("UPDATE failed: %s", PQerrorMessage(con));
                PQclear(res);
                sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1807, ENTER);
                send(soc, sendBuf, sendLen, 0);
            }
            return 0;
        }
    }

    //トランジション終了
    res = PQexec(con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "%s %d %s", ER_STAT, E_CODE_1801, ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    // "処理が完了しました。"と送信する
    sprintf(sendBuf, "処理が完了しました。%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    return 0;

}