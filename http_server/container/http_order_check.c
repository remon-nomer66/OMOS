#include "omos_http.h"

int http_order_check(pthread_t selfId, PGconn *con, int soc, int *s_info, int layer, char *http_body){
    char sql[BUFSIZE], menu_buf[LONG_BUFSIZE], buf[LONG_BUFSIZE];
    PGresult *res;
    int menu_info[100][2];  //[0]: menu_id，[1]: 押し
    int resultRows, menu_len;

    http_body[0] = '\0';

    //層番号から商品番号を獲得
    sprintf(sql, "SELECT * FROM push_t WHERE layer = %d", &layer);
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
        menu_info[i][0] = atoi(PQgetvalue(res, i, 0));
        if((atoi(PQgetvalue(res, i, 1)) == 1) || (atoi(PQgetvalue(res, i, 2)) == 1) || (atoi(PQgetvalue(res, i, 3)) == 1)){
            menu_info[i][1] = 1;
        }
    }

    //商品番号から商品名，価格を受け取り表示
    if(layer == 1){
        sprintf(menu_buf, "<p>現在ご覧のメニューは\"コモンメニュー\"です</p>");
    }else if(layer == 2){
        sprintf(menu_buf, "<p>現在ご覧のメニューは\"ブランドメニュー\"です</p>";
    }else if(layer == 3){
        sprintf(menu_buf, "<p>現在ご覧のメニューは\"ショップメニュー\"です</p>");
    }else if(layer == 4){
        sprintf(menu_buf, "<p>現在ご覧のメニューは\"リージョナルメニュー\"です</p>");
    }else{
        sprintf(menu_buf, "<p>現在ご覧のメニューは\"シーズンメニュー\"です</p>");
    }
    sprintf(buf, "<p>利用可能なメニューは以下のとおりです</p><p>商品番号　　　商品名　　　　　価格</p><form id=\"order\">");
    strcat(menu_buf, buf);
    for(i = 0; i < menu_len; i++){
        sprintf(sql, "SELECT menu_price_t.menu_id, menu_price_t.price, recipe_t.menu_name FROM recipe_t, menu_price_t, menu_storage_t WHERE menu_id = %d AND menu_storage_t.store_id = %d", menu_info[i][0], s_info[0]);
        res = PQexec(con, sql);
        if(PQresultStatus(res) != PGRES_TUPLES_OK){
            printf("%s", PQresultErrorMessage(res));
            return -1;
        }
        resultRows = PQntuples(res);
        if(resultRows != 1){
            return -1;
        }
        if(menu_info[i][1] == 1){
           sprintf(buf, "<label for=\"mid-%d\">%s：%s　✔</label><select class=\"order\" name=\"mid-%d\"></select><br>", menu_info[i][0], PQgetvalue(res, 0, 2), atoi(PQgetvalue(res, 0, 1)), menu_info[i][0]); 
        }else{
            sprintf(buf, "<label for=\"mid-%d\">%s：%s</label><select class=\"order\" name=\"mid-%d\"></select><br>", menu_info[i][0], PQgetvalue(res, 0, 2), atoi(PQgetvalue(res, 0, 1)), menu_info[i][0]); 
        }
        strcat(menu_buf, buf);
    }
    sprintf(buf, "<input id=\"btn\" type=\"button\" value=\"送信\" /></form><p>メニューを移動する際は以下の数字を入力してください</p>");
    strcat(menu_buf, buf);
    if(layer == 1){
        sprintf(buf, "<p>\"ブランドメニュー\"：2</p><p>\"ショップメニュー\"：3</p><p>\"リージョナルメニュー\"：4</p><p>\"シーズンメニュー\"：5</p>");
    }else if(layer == 2){
        sprintf(buf, "<p>\"コモンメニュー\"：1</p><p>\"ショップメニュー\"：3</p><p>\"リージョナルメニュー\"：4</p><p>\"シーズンメニュー\"：5</p>");
    }else if(layer == 3){
        sprintf(buf, "<p>\"コモンメニュー\"：1\"ブランドメニュー\"：2</p><p>\"リージョナルメニュー\"：4</p><p>\"シーズンメニュー\"：5</p>");
    }else if(layer == 4){
        sprintf(buf, "<p>\"コモンメニュー\"：1\"ブランドメニュー\"：2</p><p>\"ショップメニュー\"：3</p><p>\"シーズンメニュー\"：5</p>");
    }else{
        sprintf(buf, "<p>\"コモンメニュー\"：1\"ブランドメニュー\"：2</p><p>\"ショップメニュー\"：3</p><p>\"リージョナルメニュー\"：4</p>");
    }
    strcat(menu_buf, buf);
    strcat(http_body, menu_buf);
    return 0;
}