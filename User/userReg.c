#include "OMOS.h"

int userReg(PGconn *__con){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int phoneNum;  //電話番号
    char userPass;  //パスワード
    char userName;  //氏名
    int point;  //ポイント
    double point_rate;  //ポイントレート
    int auth;  //権限情報


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

    while(1){
        //パスワードを入力してもらう
        sprintf(sendBuf, "パスワードを入力してください。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力が8文字以上16文字以下の場合、パスワードとして扱う
        if( 8 <= strlen(recvBuf) && strlen(recvBuf) <= 16 ){
            userPass = recvBuf;  //文字列を数値に変換
            break;
        }else{
            sprintf(sendBuf, "パスワードが不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }

    while(1){
        //氏名を入力してもらう
        sprintf(sendBuf, "氏名を入力してください。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする

        //入力が30文字以上の場合、氏名として扱う
        if( strlen(recvBuf) <= 30 ){
            userName = recvBuf;  //文字列を数値に変換
            break;
        }else{
            sprintf(sendBuf, "氏名は30文字を超えないようにしてください。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }

    while(1){
        //DB接続
        sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s", dbHost, dbPort, dbName, dbLogin, dbPwd);
        PGconn *con = PQconnectdb(connInfo);

        //DB接続失敗時、再度DB接続を試みる
        if( PQstatus(con) == CONNECTION_BAD ){
            printf("Connection to database '%s:%s %s' failed.\n", dbHost, dbPort, dbName);
            printf("%s", PQerrorMessage(con));
            con = NULL;
            sendLen = sprintf(sendBuf, "error occured%s", ENTER);
            send(__lsoc, sendBuf, sendLen, 0);
            break;
        //DB接続成功時
        }else{
            printf("Connected to database %s:%s %s\n", dbHost, dbPort, dbName);
        }
    }

    //じゃんけんを行い、勝ったら1000pt,あいこは500pt,負けは300ptを付与する
    point = janken();
    point_rate = 1.0;

    //useridを電話番号をランダム関数の種にして、生成する。useridはchar型である。
    srand(phoneNum);
    char userid = rand();

    //権限情報authを1にする
    auth = 1;

    while(1){
        //DBにユーザ情報を登録する
        sprintf(sql, "INSERT INTO user VALUES('%c', '%d', '%s', '%s', '%d', '%f', '%d')", userid, phoneNum, userPass, userName, point, point_rate, auth);
        res = PQexec(__con, sql);
        //INSERTコマンドが失敗した場合、再度INSERTコマンドを実行する
        if( PQresultStatus(res) != PGRES_COMMAND_OK ){
            printf("INSERT command failed\n");
            PQclear(res);
            PQfinish(__con);
            continue;
        }
        //INSERTコマンドが成功した場合、ループを抜ける
        else{
            printf("INSERT command OK\n");
            PQclear(res);
            PQfinish(__con);
            break;
        }
    }
}

