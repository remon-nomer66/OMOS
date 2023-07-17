#include "omos.h"

int menuDel(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen; //送受信データ長
    int u_id, u_auth, u_store, delid, changestore, changeid, i; //ユーザID、ユーザの持つ権限、ユーザの所属、削除したいメニューID、情報を変更したい店舗ID、情報を変更したいメニューID、ループカウンタ
    char response; //クライアントからの返答
    PGresult *res; //PGresult型の変数resを宣言

    u_id = u_info[0]; //ユーザID
    u_auth = u_info[1]; //ユーザの持つ権限
    u_store = u_info[2]; //ユーザの所属

    if(u_auth == AMGR){
        //あなたが削除できるメニュー一覧です。と表示
        sprintf(sendBuf, "あなたが削除できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        //テーブル名：menu_charge_tからaccount_idがu_idと一致し、かつテーブル名：push_tでlayerの値が4のもののmenu_idを取得し、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT menu_id, menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d) AND menu_id IN (SELECT menu_id FROM push_t WHERE layer = 4);", u_id); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //実行結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s: %s%s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
        //削除したい商品IDを入力してください。と表示
        sprintf(sendBuf, "削除したい商品ID（4桁）を入力してください．（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';
        //4文字以外の場合はエラーを返す
        if(recvLen != 4){
            sprintf(sendBuf, "商品IDは4桁で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(int i = 0; i < 4; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sprintf(sendBuf, "商品IDは数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }
        //受信した値をdelidに代入
        sscanf(recvBuf, "%d", &delid);
        //テーブル名：recipe_tからテーブル名：menu_charge_tからaccount_idがu_idと一致するmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d);", u_id); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
        sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
        res = PQexec(con, sendBuf); //SQL文実行
        if(PQntuples(res) == 0){ //menu_idが存在しない場合
            sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
        //実行しようとしている商品名が正しいか確認
        sprintf(sendBuf, "本当に%sを削除しますか？(y/n)%s%s", PQgetvalue(res, delid, 0), ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //受信した内容をresponseに入れる
        sscanf(recvBuf, "%s", &response);
        if(response == 'y'){ //削除する場合
            //テーブル名：recipe_t, price_charge_t, push_t, menu_storage_t, menu_charge_tからテーブル名：menu_charge_tからaccount_idがu_idと一致するmenu_idのものを削除
            sprintf(sendBuf, "DELETE FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d);", u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            sprintf(sendBuf, "DELETE FROM price_charge_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d);", u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            sprintf(sendBuf, "DELETE FROM push_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d);", u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            sprintf(sendBuf, "DELETE FROM menu_storage_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d);", u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            sprintf(sendBuf, "DELETE FROM menu_charge_t WHERE account_id = %d;", u_id); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
        }else if(response == 'n'){ //削除しない場合
            sprintf(sendBuf, "削除を中止しました．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }else{
            sprintf(sendBuf, "入力が不正です．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
    }else if(u_auth == AHQ){
        //情報を変更したいものがショップメニューかどうかを聞く。
        sprintf(sendBuf, "情報を変更したいものはショップメニューですか？%s yes または no%s%s", ENTER, ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf); //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        //受信した内容をresponseに代入
        sscanf(recvBuf, "%s", &response);
        if(strcmp(&response, "yes") == 0){
            //削除を実行したい店舗ID（2桁）を聞く。
            sprintf(sendBuf, "削除を実行したい店舗ID（2桁）を入力してください。（例：01）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //2文字以外の場合はエラーを返す
            if(recvLen != 2){
                sprintf(sendBuf, "店舗IDは2桁で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < 2; i++){
                if(isdigit(recvBuf[i]) == 0){
                    sprintf(sendBuf, "店舗IDは数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //受信した値をchangestoreに代入
            sscanf(recvBuf, "%d", &changestore);
            //削除を実行したい店舗IDが存在するかどうかをテーブル名store_tから確認
            sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //テーブル名store_tから取得した値が0の場合、店舗IDが存在しないことを表示
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "その店舗IDは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            sprintf(sendBuf, "あなたが削除できるメニュー一覧です．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            //情報を変更したい店舗IDが存在する場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致するかつテーブル名：push_tでlayerの値が4のもののmenu_idを持つmenu_nameをテーブル名：recipe_tから取得して表示
            sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d) AND menu_id IN (SELECT menu_id FROM push_t WHERE layer = 4);", changestore); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            //実行したSQL文の結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s%s", PQgetvalue(res, i, 0), ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。（例：0001）%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';
            //4文字以外の場合はエラーを返す
            if(recvLen != 4){
                sprintf(sendBuf, "商品IDは4桁で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < 4; i++){
                if(isdigit(recvBuf[i]) == 0){
                    sprintf(sendBuf, "商品IDは数字で入力してください．%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    return -1;
                }
            }
            //クライアントから受信した値をdelidに代入
            sscanf(recvBuf, "%d", &delid);
            //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
            sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
            res = PQexec(con, sendBuf); //SQL文実行
            if(PQntuples(res) == 0){ //menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
            //実行しようとしている商品名が正しいか確認
            sprintf(sendBuf, "本当に%sを削除しますか？(y/n)%s%s", PQgetvalue(res, delid, 0), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            //クライアントから受信した値をresponseに代入
            sscanf(recvBuf, "%s", &response);
            if(response == 'y'){ //削除する場合
                //テーブル名：recipe_t, price_charge_t, push_t, menu_storage_t, menu_charge_tからテーブル名：menu_charge_tからaccount_idがu_idと一致するmenu_idのものを削除
                sprintf(sendBuf, "DELETE FROM recipe_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                sprintf(sendBuf, "DELETE FROM price_charge_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                sprintf(sendBuf, "DELETE FROM push_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                sprintf(sendBuf, "DELETE FROM menu_storage_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                sprintf(sendBuf, "DELETE FROM menu_charge_t WHERE menu_id = %d;", changeid); //SQL文作成
                res = PQexec(con, sendBuf); //SQL文実行
                sprintf(sendBuf, "削除しました．%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else if(response == 'n'){ //削除しない場合
                sprintf(sendBuf, "削除を中止しました．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
            }else{
                sprintf(sendBuf, "入力が不正です．%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                return -1;
            }
        }else if(strcmp(&response, "no") == 0){
            sprintf(sendBuf, "削除を中止しました．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }else{
            sprintf(sendBuf, "入力が不正です．%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            return -1;
        }
    }
    return 0;
}