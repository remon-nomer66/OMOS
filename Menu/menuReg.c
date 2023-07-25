#include "omos.h"

int menuReg(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen; //送受信データ長
    int newmid, newmprice, newmstar, newmstock, newmlimit, newmlevel, newmseason, newmregion, fod; //新規登録する商品ID, 価格, 評価, 初期在庫数, メニューレベル、、店舗id、季節、チェーン番号、フードかドリンクか
    int u_id, u_auth, u_store; //ユーザID, 権限, 所属
    int i; //ループカウンタ
    char newmname[BUFSIZE], newmrecipe[LONG_BUFSIZE]; //新規登録する商品名
    PGresult *res; //PGresult型の変数resを宣言

    u_id = u_info[0]; //ユーザID
    u_auth = u_info[1]; //ユーザの持つ権限
    u_store = u_info[2]; //ユーザの所属

    if(u_auth == AMGR){
        //登録したい商品IDを入力してください。
        sendLen = sprintf(sendBuf, "登録したい商品ID（4桁：半角数字）を入力してください。（例：0001）%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品IDを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //4文字以外の場合はエラーを返す。
        if(strlen(recvBuf) != 4){
            sendLen = sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < strlen(recvBuf); i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "商品IDは半角数字で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmidに格納
        sscanf(recvBuf, "%d", &newmid);
        //newmidと同じmenu_idがテーブル名：push_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM push_t WHERE menu_id = %d;", newmid);
        res = PQexec(con, sendBuf);
        //存在している場合は既に使われていることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に使われている商品IDです。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        PQclear(res);
        //フードかドリンクかを聞く。フードなら0を、ドリンクなら1を入力させる。
        sendLen = sprintf(sendBuf, "登録する商品がフードなら0を、ドリンクなら1を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //数字以外が入力されていないかを確認
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をfodに格納
        sscanf(recvBuf, "%d", &fod);
        //fodが0か1であるかを確認。どちらでもない場合はエラーを返す。
        if(fod != 0 && fod != 1){
            sendLen = sprintf(sendBuf, "フードなら0を、ドリンクなら1を入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //商品名を入力してください。
        sendLen = sprintf(sendBuf, "商品名を入力してください。（例：たこわさ）%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品名を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力をnewmnameに格納
        sscanf(recvBuf, "%s", newmname);
        //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", newmname);
        res = PQexec(con, sendBuf);
        //存在している場合は、存在していることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に登録されている商品名です。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        PQclear(res);
        //新商品のレシピを入力してください。
        sendLen = sprintf(sendBuf, "新商品のレシピを入力してください。（例：たこを切って，わさびを和えます）%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品レシピを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力をnewmrecipeに格納
        sscanf(recvBuf, "%s", newmrecipe);
        //新商品の価格を入力してください。
        sendLen = sprintf(sendBuf, "新商品の価格を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品価格を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力をnewmpriceに格納
        sscanf(recvBuf, "%d", &newmprice);
        //新商品の価格が0以上であるかを確認。
        if(newmprice < 0){
            sendLen = sprintf(sendBuf, "価格は0以上で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "価格は半角数字で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //押しにして登録するかどうかを確認。するなら１、しないなら０を入力してください。
        sendLen = sprintf(sendBuf, "押しメニューとして登録しますか？するなら１、しないなら０を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //押しにするかどうかを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //数字以外が入力されていないかを確認
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmstarに格納
        sscanf(recvBuf, "%d", &newmstar);
        //newstarが0か1であるかを確認。どちらでもない場合はエラーを返す。
        if(newmstar != 0 && newmstar != 1){
            sendLen = sprintf(sendBuf, "押しメニューにするかどうかは１か０で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //初期在庫数を入力してください。
        sendLen = sprintf(sendBuf, "初期在庫数を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //初期在庫数を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        for(i=0; i<recvLen-1; i++){
            if(!isdigit(recvBuf[i])){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmstockに格納
        sscanf(recvBuf, "%d", &newmstock);
        //在庫下限を入力してください。
        sendLen = sprintf(sendBuf, "在庫下限を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //在庫下限を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        for(i=0; i<recvLen-1; i++){
            if(!isdigit(recvBuf[i])){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmlimitに格納
        sscanf(recvBuf, "%d", &newmlimit);
        //テーブル名：recipe_tのmenuidにnewmidを、menu_nameにnewmnameを、recipeにnewmrecipeを、fodにfodを挿入
        sprintf(sendBuf, "INSERT INTO recipe_t VALUES(%d, '%s', '%s', %d);", newmid, newmname, newmrecipe, fod);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_price_tのmenuidにnewmidを、priceにnewmpriceを挿入
        sprintf(sendBuf, "INSERT INTO menu_price_t VALUES(%d, %d);", newmid, newmprice);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：push_tのmenuidにnewmidを、push_mgrにnewmstarを挿入
        sprintf(sendBuf, "INSERT INTO push_t VALUES(%d, 0, 0, %d);", newmid, newmstar);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_storage_tのmenuidにnewmidを、store_idにu_storeを、storageにnewmstockを、min_storageにnewmlimitを挿入
        sprintf(sendBuf, "INSERT INTO menu_storage_t VALUES(%d, %d, %d, %d, 0);", newmid, u_store, newmstock, newmlimit);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_charge_tのmenuidにnewmidを、user_idにu_idを挿入
        sprintf(sendBuf, "INSERT INTO menu_charge_t VALUES(%d, %d);", newmid, u_id);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_detail_tのmenuidにnewmidを、layerに3を、idにu_storeを、seasonに0を挿入
        sprintf(sendBuf, "INSERT INTO menu_detail_t VALUES(%d, 3, %d, 0);", newmid, u_store);
        res = PQexec(con, sendBuf);
        PQclear(res);
    }else if(u_auth == AHQ){
        //登録したい商品IDを入力してください。
        sendLen = sprintf(sendBuf, "登録したい商品ID（4桁：半角数字）を入力してください。（例：0001）%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品IDを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //4文字以外の場合はエラーを返す。
        if(strlen(recvBuf) != 4){
            sendLen = sprintf(sendBuf, "商品IDは4桁：半角数字で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < strlen(recvBuf); i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "商品IDは半角数字で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmidに格納
        sscanf(recvBuf, "%d", &newmid);
        //newmidと同じmenu_idがテーブル名：push_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM push_t WHERE menu_id = %d;", newmid);
        res = PQexec(con, sendBuf);
        //存在している場合は既に使われていることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に使われている商品IDです。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        PQclear(res);
        //フードかドリンクかを聞く。フードなら0を、ドリンクなら1を入力させる。
        sendLen = sprintf(sendBuf, "登録する商品がフードなら0を、ドリンクなら1を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //数字以外が入力されていないかを確認
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をfodに格納
        sscanf(recvBuf, "%d", &fod);
        //fodが0か1であるかを確認。どちらでもない場合はエラーを返す。
        if(fod != 0 && fod != 1){
            sendLen = sprintf(sendBuf, "フードなら0を、ドリンクなら1を入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //商品名を入力してください。
        sendLen = sprintf(sendBuf, "商品名を入力してください。（例：たこわさ）%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品名を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力をnewmnameに格納
        sscanf(recvBuf, "%s", newmname);
        //商品名と同じmenu_nameがテーブル名：recipe_tにいないかを確認。
        sprintf(sendBuf, "SELECT * FROM recipe_t WHERE menu_name = '%s';", newmname);
        res = PQexec(con, sendBuf);
        //存在している場合は、存在していることを伝える。
        if(PQntuples(res) != 0){
            sendLen = sprintf(sendBuf, "既に登録されている商品名です。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        PQclear(res);
        //新商品のレシピを入力してください。
        sendLen = sprintf(sendBuf, "新商品のレシピを入力してください。（例：たこを切って，わさびを和えます）%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品レシピを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力をnewmrecipeに格納
        sscanf(recvBuf, "%s", newmrecipe);
        //新商品の価格を入力してください。
        sendLen = sprintf(sendBuf, "新商品の価格を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //商品価格を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力をnewmpriceに格納
        sscanf(recvBuf, "%d", &newmprice);
        //新商品の価格が0以上であるかを確認。
        if(newmprice < 0){
            sendLen = sprintf(sendBuf, "価格は0以上で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力された文字が数字以外ならエラーを返す。
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "価格は半角数字で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //押しにして登録するかどうかを確認。するなら１、しないなら０を入力してください。
        sendLen = sprintf(sendBuf, "押しメニューとして登録しますか？するなら１、しないなら０を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //押しにするかどうかを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //数字以外が入力されていないかを確認
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmstarに格納
        sscanf(recvBuf, "%d", &newmstar);
        //newstarが0か1であるかを確認。どちらでもない場合はエラーを返す。
        if(newmstar != 0 && newmstar != 1){
            sendLen = sprintf(sendBuf, "押しメニューにするかどうかは１か０で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //どのメニューレベルで登録しますか？ 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．
        sendLen = sprintf(sendBuf, "どのメニューレベルで登録しますか？%s 選択肢は1：コモンメニュー、2：ブランドメニュー、3：ショップメニュー、4：リージョンメニュー、5：シーズンメニューです．%s%s", ENTER, ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //メニューレベルを受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //数字以外が入力されていないかを確認
        for(i = 0; i < recvLen-1; i++){
            if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmlevelに格納
        sscanf(recvBuf, "%d", &newmlevel);
        //newmlevelの値が1以上かつ5以下であるかを確認。
        if(newmlevel < 1 || newmlevel > 5){
            sendLen = sprintf(sendBuf, "メニューレベルは1以上5以下で入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }if(newmlevel == 3){
            //どの店舗のメニューなのかを確認。
            sendLen = sprintf(sendBuf, "登録する店舗の店舗ID（3桁：半角数字）を入力してください。%s%s", ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
            //店舗IDを受信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
            recvBuf[recvLen-1] = '\0';
            //3文字以外の場合はエラーを返す
            if(strlen(recvBuf) != 3){
                sendLen = sprintf(sendBuf, "店舗IDは3桁：半角数字で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
            //入力された文字が数字以外ならエラーを返す。
            for(i = 0; i < 3; i++){
                if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                    sendLen = sprintf(sendBuf, "店舗IDは半角数字で入力してください。%s", ENTER);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }
            }
            //入力をu_storeに格納
            sscanf(recvBuf, "%d", &u_store);
            //テーブル名：store_tのstore_idにu_storeがあるかを確認。
            sprintf(sendBuf, "SELECT * FROM store_t WHERE store_id = %d;", u_store);
            res = PQexec(con, sendBuf);
            //存在していない場合はエラーを返す。
            if(PQntuples(res) == 0){
                sendLen = sprintf(sendBuf, "存在しない店舗IDです。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
            PQclear(res);
            //newmseasonの値を0にする。
            newmseason = 0;
            //newmregionの値をu_storeにする。
            newmregion = u_store;
        }else if(newmlevel == 4){
            //どの地域のメニューなのかを入力してくださいと聞く。打ち込むのは01～49の2桁の数字。
            sendLen = sprintf(sendBuf, "登録する地域の地域ID（2桁：半角数字、01 ~ 49）を入力してください。%s%s", ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
            //地域IDを受信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
            recvBuf[recvLen-1] = '\0';
            //数字以外が入力されていないかを確認
            for(i = 0; i < recvLen-1; i++){
                if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                    sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }
            }
            //入力をnewmregionに格納
            sscanf(recvBuf, "%d", &newmregion);
            //newmregionの値が1以上かつ49以下であるかを確認。
            if(newmregion < 1 || newmregion > 49){
                sendLen = sprintf(sendBuf, "地域IDは1以上49以下で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
            //テーブル名：region_tのregion_idにnewmregionがあるかを確認。
            sprintf(sendBuf, "SELECT * FROM region_t WHERE region_id = %d;", newmregion);
            res = PQexec(con, sendBuf);
            //存在していない場合はエラーを返す。
            if(PQntuples(res) == 0){
                sendLen = sprintf(sendBuf, "存在しない地域IDです。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
            PQclear(res);
        }else if(newmlevel == 5){ //newmlevelの値が5の場合はどのシーズンにするかを聞く。
            //どのシーズンにするかを確認。
            sendLen = sprintf(sendBuf, "どのシーズンにするかを入力してください。%s 選択肢は1：春、2：夏、3：秋、4：冬です。%s%s", ENTER, ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
            //シーズンを受信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0);
            recvBuf[recvLen-1] = '\0';
            //数字以外が入力されていないかを確認
            for(i = 0; i < recvLen-1; i++){
                if(recvBuf[i] < '0' || recvBuf[i] > '9'){
                    sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                    send(soc, sendBuf, sendLen, 0);
                    return -1;
                }
            }
            //入力をnewmseasonに格納
            sscanf(recvBuf, "%d", &newmseason);
            //newmseasonの値が1以上かつ4以下であるかを確認。
            if(newmseason < 1 || newmseason > 4){
                sendLen = sprintf(sendBuf, "シーズンは1以上4以下で入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
            //newmregionの値をu_storeにする。
            newmregion = u_store;
        }else{
            //newmseasonの値を0にする。
            newmseason = 0;
            //newmregionの値をu_storeにする。
            newmregion = u_store;
        }
        //初期在庫数を入力してください。
        sendLen = sprintf(sendBuf, "初期在庫数を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //初期在庫数を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        for(i=0; i<recvLen-1; i++){
            if(!isdigit(recvBuf[i])){
                sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
                send(soc, sendBuf, sendLen, 0);
                return -1;
            }
        }
        //入力をnewmstockに格納
        sscanf(recvBuf, "%d", &newmstock);
        //在庫下限を入力してください。
        sendLen = sprintf(sendBuf, "在庫下限を入力してください。%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
        //在庫下限を受信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);
        recvBuf[recvLen-1] = '\0';
        //入力された文字が数字以外ならエラーを返す。
        if(!isdigit(recvBuf[0])){
            sendLen = sprintf(sendBuf, "数字を入力してください。%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
            return -1;
        }
        //入力をnewmlimitに格納
        sscanf(recvBuf, "%d", &newmlimit);
        //テーブル名：recipe_tのmenuidにnewmidを、menu_nameにnewmnameを、recipeにnewmrecipeを、fodにfodを挿入
        sprintf(sendBuf, "INSERT INTO recipe_t VALUES(%d, '%s', '%s', %d);", newmid, newmname, newmrecipe, fod);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_price_tのmenuidにnewmidを、priceにnewmpriceを挿入
        sprintf(sendBuf, "INSERT INTO menu_price_t VALUES(%d, %d);", newmid, newmprice);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：push_tのmenuidにnewmidを、push_hqにnewmstarを挿入
        sprintf(sendBuf, "INSERT INTO push_t VALUES(%d, %d, 0, 0);", newmid, newmstar);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_storage_tのmenuidにnewmidを、store_idにu_storeを、storageにnewmstockを、min_storageにnewmlimitを挿入、storage_flagに0を挿入
        sprintf(sendBuf, "INSERT INTO menu_storage_t VALUES(%d, %d, %d, %d, 0);", newmid, u_store, newmstock, newmlimit);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_charge_tのmenuidにnewmidを、user_idにu_idを挿入
        sprintf(sendBuf, "INSERT INTO menu_charge_t VALUES(%d, %d);", newmid, u_id);
        res = PQexec(con, sendBuf);
        PQclear(res);
        //テーブル名：menu_detail_tのmenuidにnewmidを、layerにnewmlevelを、idにnewmregionを、seasonにnewmseasonを挿入
        sprintf(sendBuf, "INSERT INTO menu_detail_t VALUES(%d, %d, %d, %d);", newmid, newmlevel, newmregion, newmseason);
        res = PQexec(con, sendBuf);
        PQclear(res);
    }
    sprintf(sendBuf, "操作を終了します。%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    return 0;
}