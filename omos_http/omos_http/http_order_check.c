#include "omos_http.h"

int http_order_check(pthread_t selfId, PGconn *con, int soc, int *s_info, char command[][BUFSIZE], char param[][BUFSIZE], char *http_body){
    char sql[BUFSIZE], menu_buf[LONG_BUFSIZE], buf[LONG_BUFSIZE];
    PGresult *res;
    int menu_info[BUFSIZE][3];  //[0]: menu_id，[1]: 押し，[2]在庫個数
    int tmp_menu_info[BUFSIZE][3];
    int resultRows, menu_len, i, j, layer, season, id;
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    int chain_id, region_id;

    http_body[0] = '\0';

    layer = atoi(param[1]);

    //層番号から商品番号を獲得
    if(layer == 1){
        //メニューIDの取得
        sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE layer = 1");
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
            return -1;
        }
        id = atoi(PQgetvalue(res, 0, 0));

        //メニューIDの取得
        sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id = %d", id);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
            return -1;
        }
        id = atoi(PQgetvalue(res, 0, 0));

        //メニューIDの取得
        sprintf(sql, "SELECT menu_id FROM menu_detail_t WHERE id = %d", id);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
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
            return -1;
        }
        menu_len = resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
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
        printf("%s\n", sql);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        menu_len = resultRows = PQntuples(res);
        if(resultRows == 0){
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
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows == 0){
            return -1;
        }
        if(atoi(PQgetvalue(res, 0, 0)) == 1 || atoi(PQgetvalue(res, 0, 1)) == 1 || atoi(PQgetvalue(res, 0, 2)) == 1){
            menu_info[i][1] = 1;
        }else{
            menu_info[i][1] = 0;
        }
    }

    printf("flag1\n");

    //商品番号から商品名，価格を受け取り表示
    if(layer != 6){
        sprintf(http_body, "<form id=\"order\"><table border=\"1\"><thead><tr><th></th><th>商品名</th><th>価格</th><th>おすすめ</th><th>残数</th></tr></thead><tbody>");
    }else{
        sprintf(http_body, "<form id=\"order\"><table border=\"1\"><thead><tr><th></th><th>商品名</th><th>価格</th><th>残数</th></tr></thead><tbody>");
    }

    for(i = 0; i < j; i++){
        sprintf(sql, "SELECT recipe_t.menu_name, menu_price_t.price FROM recipe_t JOIN menu_price_t ON recipe_t.menu_id = menu_price_t.menu_id WHERE recipe_t.menu_id = %d", menu_info[i][0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
            return -1;
        }
        if(layer != 6){
            if(menu_info[i][1] == 1){
                sprintf(http_body + strlen(http_body), "<tr><td><input name=\"mid-%d\" id=\"mid-%d-t\" type=\"checkbox\" class=\"order-c\" onclick=\"pClickEvent(event)\"></td><td>%s</td><td>%d</td><td>✔</td><td>%d</td></tr>", menu_info[i][0], menu_info[i][0], PQgetvalue(res, 0, 0), atoi(PQgetvalue(res, 0, 1)), menu_info[i][2]); 
            }else{
                sprintf(http_body + strlen(http_body), "<tr><td><input name=\"mid-%d\" id=\"mid-%d-t\" type=\"checkbox\" class=\"order-c\" onclick=\"pClickEvent(event)\"></td><td>%s</td><td>%d</td><td></td><td>%d</td></tr>", menu_info[i][0], menu_info[i][0], PQgetvalue(res, 0, 0), atoi(PQgetvalue(res, 0, 1)), menu_info[i][2]); 
            }
        }else{
            if(menu_info[i][1] == 1){
                sprintf(http_body + strlen(http_body), "<tr><td><input name=\"mid-%d\" id=\"mid-%d-t\" type=\"checkbox\" class=\"order-c\" onclick=\"pClickEvent(event)\"></td><td>%s</td><td>%d</td><td>%d</td></tr>", menu_info[i][0], menu_info[i][0], PQgetvalue(res, 0, 0), atoi(PQgetvalue(res, 0, 1)), menu_info[i][2]); 
            }
        }
    }
    sprintf(http_body + strlen(http_body), "</tbody></table></form>");
    return 0;
}
