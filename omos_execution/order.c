#include "omos.h"
#include "order.h"

int order(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    int layer = 6;     //メニューレベル(1は初期値)
    char sql[BUFSIZE], check_buf[BUFSIZE], buf[BUFSIZE];
    PGresult *res;
    int menu_info[BUFSIZE][3];  //[0]: menu_id，[1]: 押し，[2]在庫個数
    int tmp_menu_info[BUFSIZE][3];
    int i, j, k, resultRows, menu_len, cnt;
    int param1, param2;
    int order_con[5][3] = {0};    //[0]: 商品番号，[1]: 個数，[2]在庫個数
    int flag = 0;
    char comm[BUFSIZE];
    int e_auth[2];
    int id, season, len, int_buf1, int_buf2, int_buf3;
    int order_cnt = 0;
    char layer_name[7][30] = {"コモンメニュー", "ブランドメニュー", "ショップメニュー", "リージョナルメニュー", "シーズンメニュー", "推しメニュー", "全種類"};
    int chain_id, region_id;
    int act = 0;

    while(1){

        sprintf(sendBuf, "注文する際は\"(商品番号) (個数)\"と入力してください%s注文の種類数は5種類以下，個数は注文可能個数以下でご注文ください．%s商品及び個数の指定が全て終了しましたら，\"OK\"を入力してください%s履歴を確認する場合は\"HIST\"，会計を行う場合は\"PAY\"を入力してください%s", ENTER, ENTER, ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

        while(1){
            time_t t = time(NULL);
            struct tm *local = localtime(&t);

            memset(menu_info, 0, sizeof(menu_info));
            if(layer == 1){
                //メニューIDの取得
                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE layer = 1");
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1401, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows;
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1 && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }else if(layer == 2){
                //チェーン番号の取得
                sprintf(sql, "SELECT chain_id FROM chain_t WHERE store_id = %d", s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1402, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                id = atoi(PQgetvalue(res, 0, 0));

                //メニューIDの取得
                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id = %d", id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1401, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows;
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1 && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }else if(layer == 3){
                //メニューIDの取得
                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id = %d", s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1401, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows;
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1  && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }else if(layer == 4){
                //地域番号の取得
                sprintf(sql, "SELECT region_id FROM region_t WHERE store_id = %d", s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1403, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                id = atoi(PQgetvalue(res, 0, 0));

                //メニューIDの取得
                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id = %d", id);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1403, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows;
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1 && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }else if(layer == 5){
                //チェーン番号の取得
                sprintf(sql, "SELECT chain_id FROM chain_t WHERE store_id = %d", s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1402, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                id = atoi(PQgetvalue(res, 0, 0));

                //season番号の取得
                if(3 <= local->tm_mon && local->tm_mon <= 5){
                    season = 1;
                }else if(6 <= local->tm_mon && local->tm_mon <= 8){
                    season = 2;
                }else if(9 <= local->tm_mon && local->tm_mon <= 11){
                    season = 3;
                }else if(12 == local->tm_mon || 1 == local->tm_mon || local->tm_mon == 2){
                    season = 4;
                }

                //メニューIDの取得
                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id = %d AND season = %d", id, season);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1401, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows;
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1 && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }else if(layer == 6){
                //IDの取得
                sprintf(sql, "SELECT chain_t.chain_id, region_t.region_id FROM chain_t JOIN region_t ON chain_t.store_id = region_t.store_id WHERE region_t.store_id = %d", s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                chain_id = atoi(PQgetvalue(res, 0, 0));
                region_id = atoi(PQgetvalue(res, 0, 1));

                //season番号の取得
                if(3 <= local->tm_mon && local->tm_mon <= 5){
                    season = 1;
                }else if(6 <= local->tm_mon && local->tm_mon <= 8){
                    season = 2;
                }else if(9 <= local->tm_mon && local->tm_mon <= 11){
                    season = 3;
                }else if(12 == local->tm_mon || 1 == local->tm_mon || local->tm_mon == 2){
                    season = 4;
                }

                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id IN (0, %d, %d, %d) AND (season = 0 OR season = %d)", chain_id, region_id, s_info[0], season);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1 && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }else{
                //IDの取得
                sprintf(sql, "SELECT chain_t.chain_id, region_t.region_id FROM chain_t JOIN region_t ON chain_t.store_id = region_t.store_id WHERE region_t.store_id = %d", s_info[0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                chain_id = atoi(PQgetvalue(res, 0, 0));
                region_id = atoi(PQgetvalue(res, 0, 1));

                //season番号の取得
                if(3 <= local->tm_mon && local->tm_mon <= 5){
                    season = 1;
                }else if(6 <= local->tm_mon && local->tm_mon <= 8){
                    season = 2;
                }else if(9 <= local->tm_mon && local->tm_mon <= 11){
                    season = 3;
                }else if(12 == local->tm_mon || 1 == local->tm_mon || local->tm_mon == 2){
                    season = 4;
                }

                sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id IN (0, %d, %d, %d) AND (season = 0 OR season = %d)", chain_id, region_id, s_info[0], season);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                menu_len = resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                for(i = 0; i < menu_len; i++){
                    tmp_menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
                }
                j = 0;
                for(i = 0; i < menu_len; i++){
                    sprintf(sql, "SELECT storage FROM menu_storage_t WHERE store_id = %d AND menu_id = %d", s_info[0], tmp_menu_info[i][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows == 1 && atoi(PQgetvalue(res, 0, 0)) != 0){
                        menu_info[j][0] = tmp_menu_info[i][0];
                        menu_info[j][2] = atoi(PQgetvalue(res, 0, 0));
                        j++;
                    }
                }
            }

            //共通(押し)
            for(i = 0; i < j; i++){
                sprintf(sql, "SELECT push_hq, push_cor, push_mgr FROM push_t WHERE menu_id = %d", menu_info[i][0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows == 0){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                if(atoi(PQgetvalue(res, 0, 0)) == 1 || atoi(PQgetvalue(res, 0, 1)) == 1 || atoi(PQgetvalue(res, 0, 2)) == 1){
                    menu_info[i][1] = 1;
                }else{
                    menu_info[i][1] = 0;
                }
            }

            //商品番号から商品名，価格を受け取り表示
            if(layer == 1){
                sprintf(sendBuf, "現在ご覧のメニューは\"コモンメニュー\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(layer == 2){
                sprintf(sendBuf, "現在ご覧のメニューは\"ブランドメニュー\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(layer == 3){
                sprintf(sendBuf, "現在ご覧のメニューは\"ショップメニュー\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(layer == 4){
                sprintf(sendBuf, "現在ご覧のメニューは\"リージョナルメニュー\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(layer == 5){
                sprintf(sendBuf, "現在ご覧のメニューは\"シーズンメニュー\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else if(layer == 6){
                sprintf(sendBuf, "現在ご覧のメニューは\"推しメニュー\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }else{
                sprintf(sendBuf, "現在ご覧のメニューは\"全種類\"です%s", ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }

            sprintf(sendBuf, "メニューを移動する際は以下の数字を入力してください%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            if(layer == 1){
                sprintf(sendBuf, "　　\"ブランドメニュー\"：2%s　　\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s　　\"シーズンメニュー\"：5%s　　　　\"推しメニュー\"：6%s　　　　　　　\"全種類\"：7%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }else if(layer == 2){
                sprintf(sendBuf, "　　　\"コモンメニュー\"：1%s　　\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s　　\"シーズンメニュー\"：5%s　　　　\"推しメニュー\"：6%s　　　　　　　\"全種類\"：7%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }else if(layer == 3){
                sprintf(sendBuf, "　　　\"コモンメニュー\"：1%s　　\"ブランドメニュー\"：2%s\"リージョナルメニュー\"：4%s　　\"シーズンメニュー\"：5%s　　　　\"推しメニュー\"：6%s　　　　　　　\"全種類\"：7%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }else if(layer == 4){
                sprintf(sendBuf, "　　　\"コモンメニュー\"：1%s　　\"ブランドメニュー\"：2%s　　\"ショップメニュー\"：3%s　　\"シーズンメニュー\"：5%s　　　　\"推しメニュー\"：6%s　　　　　　　\"全種類\"：7%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }else if(layer == 5){
                sprintf(sendBuf, "　　　\"コモンメニュー\"：1%s　　\"ブランドメニュー\"：2%s　　\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s　　　　\"推しメニュー\"：6%s　　　　　　　\"全種類\"：7%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }else if(layer == 6){
                sprintf(sendBuf, "　　　\"コモンメニュー\"：1%s　　\"ブランドメニュー\"：2%s　　\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s　　\"シーズンメニュー\"：5%s　　　　　　　\"全種類\"：7%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }else{
                sprintf(sendBuf, "　　　\"コモンメニュー\"：1%s　　\"ブランドメニュー\"：2%s　　\"ショップメニュー\"：3%s\"リージョナルメニュー\"：4%s　　\"シーズンメニュー\"：5%s　　　　\"推しメニュー\"：6%s", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER);
            }
            
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            if(layer != 6){
                sprintf(sendBuf, "利用可能なメニューは以下のとおりです%s 商品番号\t\t商品名\t\t価格\t\t推し\t\t個数%s", ENTER, ENTER);
            }else{
                sprintf(sendBuf, "利用可能なメニューは以下のとおりです%s 商品番号\t\t商品名\t\t価格\t\t個数%s", ENTER, ENTER);
            }
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            for(i = 0; i < j; i++){
                sprintf(sql, "SELECT recipe_t.menu_name, menu_price_t.price FROM recipe_t JOIN menu_price_t ON recipe_t.menu_id = menu_price_t.menu_id WHERE recipe_t.menu_id = %d", menu_info[i][0]);
                res = PQexec(con, sql);
                if(PQresultStatus(res) != PGRES_TUPLES_OK){
                    printf("%s", PQresultErrorMessage(res));
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                resultRows = PQntuples(res);
                if(resultRows != 1){
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    return -1;
                }
                if(layer != 6){
                    sprintf(sendBuf, "%8d\t%-25s\t%7d", menu_info[i][0], PQgetvalue(res, 0, 0), atoi(PQgetvalue(res, 0, 1)));
                    if(menu_info[i][1] == 1){
                        sprintf(buf, "\t\t*\t\t%4d%s", menu_info[i][2], ENTER);
                        strcat(sendBuf, buf);
                        len = strlen(buf);
                        buf[len] = '\0';
                    }else{
                        sprintf(buf, "\t\t \t\t%4d%s", menu_info[i][2], ENTER);
                        strcat(sendBuf, buf);
                    }
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }else{
                    if(menu_info[i][1] == 1){
                        sprintf(sendBuf, "%8d\t%-25s\t%7d\t\t%4d%s", menu_info[i][0], PQgetvalue(res, 0, 0), atoi(PQgetvalue(res, 0, 1)), menu_info[i][2], ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }
            }
            sprintf(sendBuf, "%s", DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

            i = flag = 0;
            //クライアントからのメッセージを解析
            while(order_cnt < 5){
                recvLen = receive_message(soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                    
                    cnt = sscanf(recvBuf, "%d %d", &param1, &param2);
                    if(cnt == 1){
                        if((1 <= param1) && (param1 <= 7)){
                            layer = param1;
                            break;
                        }else{
                            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1404, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                    }else if(cnt == 2){
                        for(i = 0; i < j; i++){
                            if(param1 == menu_info[i][0]){
                                if(layer != 6){
                                    flag = MENU_OK;
                                    if(param2 > menu_info[i][2]){
                                        flag = QUANTITY_ERR;
                                    }
                                }else if(layer == 6 && param1 == menu_info[i][0] && menu_info[i][1] == 1){
                                    flag = MENU_OK;
                                    if(param2 > menu_info[i][2]){
                                        flag = QUANTITY_ERR;
                                    }
                                }
                            }
                            for(k = 0; k < order_cnt; k++){
                                printf("order_info[%d][0]: %d\n", i, menu_info[i][0]);
                                if(order_con[k][0] == param1){
                                    flag = ALREADY;
                                }
                            }
                        }
                        if(flag == MENU_OK){
                            int_buf1 = param1;
                            int_buf2 = param2;
                            order_con[order_cnt][0] = int_buf1;     //商品番号
                            order_con[order_cnt][1] = int_buf2;     //個数
                            for(i = 0; i < j; i++){
                                if(param1 == menu_info[i][0]){
                                    int_buf3 = menu_info[i][2];
                                    order_con[order_cnt][2] = int_buf3;
                                }
                            }
                            order_cnt++;
                        }else if(flag == QUANTITY_ERR){
                            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1405, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }else if(flag == ALREADY){
                            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1406, ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }else{
                            sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_1407, ENTER, DATA_END);
                            //sprintf(sendBuf, "%s内の商品番号を入力してください%s%s", layer_name[layer - 1], ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                        if(flag == MENU_OK && order_cnt < 5){
                            sprintf(sendBuf, "入力を終了する場合は\"OK\"と入力してください%s%s", ENTER, DATA_END);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                        /* else if(flag == MENU_OK){
                            sprintf(sendBuf, "注文可能な種類の最大数に到達しました%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        } */
                    }
                    //else if(cnt == 2 && param2 > 5){
                        //sprintf(sendBuf, "商品の個数は5個以下で入力してください%s%s", ENTER, DATA_END);
                        //sendLen = strlen(sendBuf);
                        //send(soc, sendBuf, sendLen, 0);
                        //printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    //}
                    else if(cnt == 0){
                        cnt = sscanf(recvBuf, "%s", comm);
                        if(cnt == 1){
                            if(strcmp(comm, HIST) == 0){
                                history(selfId, con, soc, recvBuf, sendBuf, s_info);
                                break;
                            }else if(strcmp(comm, OK) == 0){
                                flag = ORDER_END;
                                break;
                            }else if(strcmp(comm, END) == 0){
                                printf("flag: END\n");
                                sprintf(sendBuf, "認証画面に戻ります%s%s", ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                return 0;
                            }else{
                                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_300, ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            }
                        }
                    }else{
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_300, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }else{
                    sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_300, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }

            //入力内容確認
            if(order_cnt == 5 || flag == ORDER_END){

                sprintf(check_buf, "入力された内容は以下のとおりです%s 商品番号 　　　　　商品名　　　　　 　数量%s", ENTER, ENTER);
                for(j = 0; j < order_cnt; j++){
                    sprintf(sql, "SELECT menu_name FROM recipe_t WHERE menu_id = %d", order_con[j][0]);
                    res = PQexec(con, sql);
                    if(PQresultStatus(res) != PGRES_TUPLES_OK){
                        printf("%s", PQresultErrorMessage(res));
                        sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        return -1;
                    }
                    resultRows = PQntuples(res);
                    if(resultRows != 1){
                        sprintf(sendBuf, "商品番号(%d)が誤っています%s%s", order_con[j][0], ENTER, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        flag = -1;
                        break;
                    }
                    sprintf(buf, "    %5d %10s\t\t%5d%s", order_con[j][0], PQgetvalue(res, 0, 0), order_con[j][1], ENTER);
                    strcat(check_buf, buf);
                }
                if(flag != -1){

                    sprintf(buf, "入力された内容に問題が無ければ\"OK\"を入力してください%s%s", ENTER, DATA_END);
                    strcat(check_buf, buf);
                    sendLen = strlen(check_buf);
                    send(soc, check_buf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, check_buf);
                    flag = ORDER_OK;
                }
            }
            if(flag == ORDER_OK){
                recvLen = receive_message(soc, recvBuf, BUFSIZE);
                if(recvLen > 0){
                    recvBuf[recvLen - 1] = '\0';
                    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

                    cnt = sscanf(recvBuf, "%s", comm);

                    if(cnt == 1 && (strcmp(comm, OK) == 0)){

                        PQexec(con, "BIGIN");

                        //order_tに登録
                        for(j = 0; j < order_cnt; j++){
                            sprintf(sql, "INSERT INTO order_t (store_id, desk_num, menu_id, order_cnt, kitchen_flag, order_date, order_time, user_id) VALUES (%d, %d, %d, %d, %d, '%d-%d-%d', '%d:%d:%d', %d)", s_info[0], s_info[1], order_con[j][0], order_con[j][1], 0, local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec, u_info[0]);
                            printf("%s\n", sql);
                            res = PQexec(con, sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("%s", PQresultErrorMessage(res));
                                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                PQexec(con, "ROLLBACK");
                                return -1;
                            }
                            sprintf(sql, "UPDATE menu_storage_t SET storage = %d WHERE menu_id = %d AND store_id = %d", order_con[j][2] - order_con[j][1], order_con[j][0], s_info[0]);
                            printf("%s\n", sql);
                            res = PQexec(con, sql);
                            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                                printf("%s", PQresultErrorMessage(res));
                                sprintf(sendBuf, "%s %d%s%s", ER_STAT, E_CODE_100, ENTER, DATA_END);
                                sendLen = strlen(sendBuf);
                                send(soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                PQexec(con, "ROLLBACK");
                                return -1;
                            }
                        }

                        PQexec(con, "COMMIT");

                        sprintf(sendBuf, "%s %d%s", OK_STAT, 1, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

                        sprintf(sendBuf, "注文が正常に完了しました%s", ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        order_cnt = 0;
                    }else{
                        sprintf(sendBuf, "%s %d%s", ER_STAT, E_CODE_1408, ENTER);
                        sendLen = strlen(sendBuf);
                        send(soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        order_cnt = 0;
                    }
                }
            }
            i = flag = 0;
        }
    }
    PQclear(res);
    return 0;
}