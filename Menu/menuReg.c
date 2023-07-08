#include "omos.h"

int menuReg(PGconn *__con, int __soc, int *__u_info){
    int recvLen, sendLen;　//送受信データ長
    int newmid, newmprice, newmstar, newmstock, newmlimit, newmlevel, newmstore;　//新規登録する商品ID, 価格, 評価, 初期在庫数, メニューレベル
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]　//送受信用バッファ
    char newmname; //新規登録する商品名
    pthread_t selfId = pthread_self();  //スレッド
    PGresult *res;　//PGresult型の変数resを宣言

    u_id = __u_info[0];　//ユーザID
    u_auth = __u_info[1];　//ユーザの持つ権限
    u_store = __u_info[2];　//ユーザの所属

    if(u_auth == AMGR){
        //登録したい商品IDを入力してください。
        sendLen = sprintf(sendBuf, "登録したい商品ID（4桁）を入力してください。（例：0001）%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //商品IDを受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //4文字以外の場合はエラーを返す。
        if(strlen(recvBuf) != 4){
            sendLen = sprintf(sendBuf, "商品IDは4桁で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < strlen(recvBuf); i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "商品IDは数字で入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmidに格納
        sscanf(recvBuf, "%d", &newmid);
        //newmidと同じmenu_idがテーブル名：push_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM push_t WHERE menu_id = %d;", newmid);
        res = PQexec(__con, sendBuf);
        //存在している場合は既に使われていることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に使われている商品IDです。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //商品名を入力してください。
        sendLen = sprintf(sendBuf, "商品名を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //商品名を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力をnewmnameに格納
        sscanf(recvBuf, "%s", newmname);
        //商品名と同じmenu_nameがテーブル名：menu_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM menu_t WHERE menu_name = %s;", newmname);
        res = PQexec(__con, sendBuf);
        //存在している場合は、存在していることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に登録されている商品名です。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //新商品の価格を入力してください。
        sendLen = sprintf(sendBuf, "新商品の価格を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //商品価格を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力をnewmpriceに格納
        sscanf(recvBuf, "%d", &newmprice);
        //新商品の価格が0以上であるかを確認。
        if(newmprice < 0){
            sendLen = sprintf(sendBuf, "価格は0以上で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < strlen(recvBuf); i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "価格は数字で入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //押しにして登録するかどうかを確認。するなら１、しないなら０を入力してください。
        sendLen = sprintf(sendBuf, "押しメニューとして登録しますか？するなら１、しないなら０を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //押しにするかどうかを受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力をnewmstarに格納
        sscanf(recvBuf, "%d", &newmstar);
        //newstarが0か1であるかを確認。どちらでもない場合はエラーを返す。
        if(newmstar != 0 && newmstar != 1){
            sendLen = sprintf(sendBuf, "押しメニューにするかどうかは１か０で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //初期在庫数を入力してください。
        sendLen = sprintf(sendBuf, "初期在庫数を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //初期在庫数を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        if(!isdigit(recvBuf[0])){
            sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力をnewmstockに格納
        sscanf(recvBuf, "%d", &newmstock);
        //在庫下限を入力してください。
        sendLen = sprintf(sendBuf, "在庫下限を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //在庫下限を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        if(!isdigit(recvBuf[0])){
            sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力をnewmlimitに格納
        sscanf(recvBuf, "%d", &newmlimit);
        //テーブル名：recipe_tのmenuidにnewmidを、menu_nameにnewmnameを挿入
        sprintf(sendBuf, "INSERT INTO menu_t VALUES(%d, %s);", newmid, newmname);
        res = PQexec(__con, sendBuf);
        //テーブル名：price_charge_tのmenuidにnewmidを、priceにnewmpriceを挿入
        sprintf(sendBuf, "INSERT INTO price_charge_t VALUES(%d, %d);", newmid, newmprice);
        res = PQexec(__con, sendBuf);
        //テーブル名：push_tのmenuidにnewmidを、pushにnewmstarを、layerに4を挿入
        sprintf(sendBuf, "INSERT INTO push_t VALUES(%d, %d, 4);", newmid, newmstar);
        res = PQexec(__con, sendBuf);
        //テーブル名：menu_storage_tのmenuidにnewmidを、store_idにu_storeを、storageにnewmstockを、min_storageにnewmlimitを挿入
        sprintf(sendBuf, "INSERT INTO menu_storage_t VALUES(%d, %d, %d, %d);", newmid, u_store, newmstock, newmlimit);
        res = PQexec(__con, sendBuf);
        //テーブル名：menu_charge_tのmenuidにnewmidを、account_idにu_idを挿入
        sprintf(sendBuf, "INSERT INTO menu_charge_t VALUES(%d, %d);", newmid, u_id);
        res = PQexec(__con, sendBuf);
    }else if(u_auth == AHQ){
        //登録したい商品IDを入力してください。
        sendLen = sprintf(sendBuf, "登録したい商品ID（4桁）を入力してください。（例：0001）%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //商品IDを受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //4文字以外の場合はエラーを返す
        if(recvLen != 4){
            sendLen = sprintf(sendBuf, "商品IDは4桁で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < 4; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "商品IDは数字で入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmidに格納
        sscanf(recvBuf, "%d", &newmid);
        //newmidと同じmenu_idがテーブル名：push_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM push_t WHERE menu_id = %d;", newmid);
        res = PQexec(__con, sendBuf);
        //存在している場合は既に使われていることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に使われている商品IDです。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //商品名を入力してください。
        sendLen = sprintf(sendBuf, "商品名を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //商品名を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力をnewmnameに格納
        sscanf(recvBuf, "%s", newmname);
        //商品名と同じmenu_nameがテーブル名：menu_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM menu_t WHERE menu_name = %s;", newmname);
        res = PQexec(__con, sendBuf);
        //存在している場合は、存在していることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に登録されている商品名です。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //新商品の価格を入力してください。
        sendLen = sprintf(sendBuf, "新商品の価格を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //商品価格を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < recvLen; i++){
            if(!isdigit(recvBuf[i])){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmpriceに格納
        sscanf(recvBuf, "%d", &newmprice);
        //新商品の価格が0以上であるかを確認。
        if(newmprice < 0){
            sendLen = sprintf(sendBuf, "価格は0以上で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //押しにして登録するかどうかを確認。するなら１、しないなら０を入力してください。
        sendLen = sprintf(sendBuf, "押しメニューとして登録しますか？するなら１、しないなら０を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //押しにするかどうかを受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力をnewmstarに格納
        sscanf(recvBuf, "%d", &newmstar);
        //newstarが0か1であるかを確認。どちらでもない場合はエラーを返す。
        if(newmstar != 0 && newmstar != 1){
            sendLen = sprintf(sendBuf, "押しメニューは１か０で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //どのメニューレベルで登録しますか？ 選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華、4：ショップメニューです．
        sendLen = sprintf(sendBuf, "どのメニューレベルで登録しますか？%s 選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華、4：ショップメニューです．%s", ENTER, ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //メニューレベルを受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力をnewmlevelに格納
        sscanf(recvBuf, "%d", &newmlevel);
        //newmlevelの値が0以上かつ4以下であるかを確認。
        if(newmlevel < 0 || newmlevel > 4){
            sendLen = sprintf(sendBuf, "メニューレベルは0以上4以下で入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            return -1;
        }
        //newmlevelの値が4の場合は、どの店舗のメニューなのかを確認。
        if(newmlevel == 4){
            //どの店舗のメニューなのかを確認。
            sendLen = sprintf(sendBuf, "登録する店舗の店舗ID（2桁）を入力してください。%s", ENTER);
            send(__soc, sendBuf, sendLen, 0);
            //店舗IDを受信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
            recvBuf[recvLen] = '\0';
            //2文字以外の場合はエラーを返す
            if(strlen(recvBuf) != 2){
                sendLen = sprintf(sendBuf, "店舗IDは2桁で入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < 2; i++){
                if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                    sendLen = sprintf(sendBuf, "店舗IDは数字で入力してください。%s", ENTER);
                    send(__soc, sendBuf, sendLen, 0);
                    return -1;
                }
            }
            //入力をnewmstoreに格納
            sscanf(recvBuf, "%d", &newmstore);
        }
        //初期在庫数を入力してください。
        sendLen = sprintf(sendBuf, "初期在庫数を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //初期在庫数を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < strlen(recvBuf); i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "初期在庫数は数字で入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmstockに格納
        sscanf(recvBuf, "%d", &newmstock);
        //在庫下限を入力してください。
        sendLen = sprintf(sendBuf, "在庫下限を入力してください。%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        //在庫下限を受信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < strlen(recvBuf); i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "在庫下限は数字で入力してください。%s", ENTER);
                send(__soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmlimitに格納
        sscanf(recvBuf, "%d", &newmlimit);
        //テーブル名：recipe_tのmenuidにnewmidを、menu_nameにnewmnameを挿入
        sprintf(sendBuf, "INSERT INTO menu_t VALUES(%d, %s);", newmid, newmname);
        res = PQexec(__con, sendBuf);
        //テーブル名：price_charge_tのmenuidにnewmidを、priceにnewmpriceを挿入
        sprintf(sendBuf, "INSERT INTO price_charge_t VALUES(%d, %d);", newmid, newmprice);
        res = PQexec(__con, sendBuf);
        //テーブル名：push_tのmenuidにnewmidを、pushにnewmstarを、layerにnewmlevelを挿入
        sprintf(sendBuf, "INSERT INTO push_t VALUES(%d, %d, %d);", newmid, newmstar, newmlevel);
        res = PQexec(__con, sendBuf);
        //テーブル名：menu_storage_tのmenuidにnewmidを、store_idにnewmstoreを、storageにnewmstockを、min_storageにnewmlimitを挿入
        sprintf(sendBuf, "INSERT INTO menu_storage_t VALUES(%d, %d, %d, %d);", newmid, newmstore, newmstock, newmlimit);
        res = PQexec(__con, sendBuf);
        //テーブル名：menu_charge_tのmenuidにnewmidを、account_idにu_idを挿入
        sprintf(sendBuf, "INSERT INTO menu_charge_t VALUES(%d, %d);", newmid, u_id);
        res = PQexec(__con, sendBuf);
    }
    return 0;
}