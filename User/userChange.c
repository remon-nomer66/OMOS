#include "OMOS.h"

int userChange(PGconn *__con, int __soc){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int phoneNum;  //電話番号
    char userPass;  //パスワード
    char userName;  //氏名

    //トランザクション開始
    PGresult *res = PQexec(__con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //会員照会を行う。電話番号の入力とパスワードの入力を別々に求める。
    while(1){
        //電話番号を入力してもらう
        sprintf(sendBuf, "電話番号を入力してください(09024681234)。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        
        //入力が数字11桁の場合、電話番号として扱う
        if( strlen(recvBuf) == 11 && isdigit(recvBuf) ){
            phoneNum = atoi(recvBuf);  //文字列を数値に変換
            break;
        }else{
            sprintf(sendBuf, "電話番号が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }

    //パスワードを入力してもらう
    while(1){
        sprintf(sendBuf, "パスワードを入力してください(12345678)。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力が英数字8桁の場合、パスワードとして扱う
        if( strlen(recvBuf) == 8 && isalnum(recvBuf) ){
            userPass = recvBuf;  //文字列を数値に変換
            break;
        }else{
            sprintf(sendBuf, "パスワードが不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }

    //user_tテーブルに電話番号とパスワードが一致する会員がいるか確認する。エラーが起こった際はロールバックする。
    sprintf(sendBuf, "SELECT * FROM user_t WHERE phone_num = %d AND user_pass = %s", phoneNum, userPass); //送信データ作成
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
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
    }

    //user_tテーブルに電話番号とパスワードが一致する会員がいない場合、エラーを返す。
    if(PQntuples(res) == 0){
        sprintf(sendBuf, "会員が存在しません。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
        return 0;
    }

    //user_tテーブルに電話番号とパスワードが一致する会員がいた場合、電話番号、氏名を取得し、表示する。
    phoneNum = PQgetvalue(res, 0, 0);  //電話番号を取得
    userName = PQgetvalue(res, 0, 2);  //氏名を取得
    sprintf(sendBuf, "電話番号:%s 氏名:%s%s", phoneNum, userName, ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__lsoc, sendBuf, sendLen, 0); //送信

    //変更する内容の選択を行う。0:電話番号、1:パスワード、2:氏名を数字入力してもらう。
    while(1){
        sprintf(sendBuf, "変更する内容を選択してください。%s", ENTER); //送信データ作成
        sprintf(sendBuf, "0:電話番号、1:パスワード、2:氏名%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力が数字1桁の場合、変更する内容として扱う
        if( strlen(recvBuf) == 1 && isdigit(recvBuf) ){
            int changeNum = atoi(recvBuf);  //文字列を数値に変換
            break;
        }else{
            sprintf(sendBuf, "入力が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }

    //変更する内容を入力してもらう
    while(1){
        if(changeNum == 0){
            sprintf(sendBuf, "電話番号を入力してください。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

            //入力が数字11桁の場合、電話番号として扱う
            if( strlen(recvBuf) == 11 && isdigit(recvBuf) ){
                int new_phoneNum = atoi(recvBuf);  //文字列を数値に変換
                break;
            }else{
                sprintf(sendBuf, "入力が不正です。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(__lsoc, sendBuf, sendLen, 0); //送信
            }
        }else if(changeNum == 1){
            sprintf(sendBuf, "パスワードを入力してください。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

            //入力が英数字8桁の場合、パスワードとして扱う
            if( strlen(recvBuf) == 8 && isalnum(recvBuf) ){
                int new_userPass = recvBuf;  //文字列を数値に変換
                break;
            }else{
                sprintf(sendBuf, "入力が不正です。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf
                );  //送信データ長
                send(__lsoc, sendBuf, sendLen, 0); //送信
            }
        }else if(changeNum == 2){
            sprintf(sendBuf, "氏名を入力してください。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
            recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

            //入力が30文字以下の場合、氏名として扱う
            if( strlen(recvBuf) <= 30 ){
                int new_userName = recvBuf;  //文字列を数値に変換
                break;
            }else{
                sprintf(sendBuf, "氏名は30文字を超えないようにしてください。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(__lsoc, sendBuf, sendLen, 0); //送信
            }
        }
    }

    //電話番号を変更する場合(changeNum=0)、内容を確認してもらう。変更前の電話番号(phoneNum)と変更後(new_phoneNum)を表示してもらう
    if(changeNum == 0){
        sprintf(sendBuf, "変更前の電話番号:%s%s", phoneNum, ENTER); //送信データ作成
        sprintf(sendBuf, "変更後の電話番号:%s%s", new_phoneNum, ENTER); //送信データ作成
        sprintf(sendBuf, "変更しますか？(y/n)%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力がyの場合、変更する。
        if( strcmp(recvBuf, "y") == 0 ){
            sprintf(sql, "UPDATE user_t SET phone_num = %s WHERE phone_num = %s;", new_phoneNum, phoneNum); //SQL文作成
            res = PQexec(__con, sql); //SQL文実行
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                send(__lsoc, "error occured", 14, 0); //送信
                //ロールバック
                sprintf(sql, "ROLLBACK;");
                res = PQexec(__con, sql);
                PQclear(res);
                PQfinish(__con);
            }
        }else{
            sprintf(sendBuf, "変更を中止しました。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    //パスワードを変更する場合(changeNum=1)、内容を確認してもらう。変更前のパスワード(userPass)と変更後(new_userPass)を表示してもらう
    }else if(changeNum == 1){
        sprintf(sendBuf, "変更前のパスワード:%s%s", userPass, ENTER); //送信データ作成
        sprintf(sendBuf, "変更後のパスワード:%s%s", new_userPass, ENTER); //送信データ作成
        sprintf(sendBuf, "変更しますか？(y/n)%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力がyの場合、変更する。
        if( strcmp(recvBuf, "y") == 0 ){
            sprintf(sql, "UPDATE user_t SET user_pass = %s WHERE user_pass = %s;", new_userPass, userPass); //SQL文作成
            res = PQexec(__con, sql); //SQL文実行
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                send(__lsoc, "error occured", 14, 0); //送信
                //ロールバック
                sprintf(sql, "ROLLBACK;");
                res = PQexec(__con, sql);
                PQclear(res);
                PQfinish(__con);
            }
        }else{
            sprintf(sendBuf, "変更を中止しました。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    //氏名を変更する場合(changeNum=2)、内容を確認してもらう。変更前の氏名(userName)と変更後(new_userName)を表示してもらう
    }else if(changeNum == 2){
        sprintf(sendBuf, "変更前の氏名:%s%s", userName, ENTER); //送信データ作成
        sprintf(sendBuf, "変更後の氏名:%s%s", new_userName, ENTER); //送信データ作成
        sprintf(sendBuf, "変更しますか？(y/n)%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力がyの場合、変更する。
        if( strcmp(recvBuf, "y") == 0 ){
            sprintf(sql, "UPDATE user_t SET user_name = %s WHERE user_name = %s;", new_userName, userName); //SQL文作成
            res = PQexec(__con, sql); //SQL文実行
            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                send(__lsoc, "error occured", 14, 0); //送信
                //ロールバック
                sprintf(sql, "ROLLBACK;");
                res = PQexec(__con, sql);
                PQclear(res);
                PQfinish(__con);
            }
        }else{
            sprintf(sendBuf, "変更を中止しました。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }else{
        sprintf(sendBuf, "error occured%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
    }

    //コミット
    sprintf(sql, "COMMIT;");
    res = PQexec(__con, sql);
    PQclear(res);
    PQfinish(__con);

    return 0;

}