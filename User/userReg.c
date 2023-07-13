#include "OMOS.h"

int userReg(pthread_t selfId, PGconn *__con, int __soc, int *__u_info){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    int phoneNum;  //電話番号
    char userPass;  //パスワード
    char userName;  //氏名
    int point;  //ポイント
    double point_rate;  //ポイントレート
    int auth;  //権限情報

    //トランザクション開始
    PGresult *res = PQexec(__con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(__con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    while(1){
        //電話番号を入力してもらう
        sprintf(sendBuf, "電話番号を入力してください(09024681234)。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        
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
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

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
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        //入力が30文字以下の場合、氏名として扱う
        if( strlen(recvBuf) <= 30 ){
            userName = recvBuf;  //文字列を数値に変換
            break;
        }else{
            sprintf(sendBuf, "氏名は30文字を超えないようにしてください。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__lsoc, sendBuf, sendLen, 0); //送信
        }
    }

    //じゃんけんを行い、勝ったら1000pt,あいこは500pt,負けは300ptを付与する
    point = janken(soc);
    point_rate = 1.0;

    //useridを電話番号をランダム関数の種にして、生成する。useridはchar型である。
    srand(phoneNum);
    char userid = rand();

    //権限情報authを1にする
    auth = 1;

    //user_にuserid,phoneNum,userName,userPassを登録する。うまくいかなかった場合、ロールバックする。
    sprintf(sendBuf, "INSERT INTO user_(user_id, user_phone, user_name, user_pass) VALUES(%c, %d, %s, %s)", userid, phoneNum, userName, userPass); //送信データ作成
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("INSERT failed: %s", PQerrorMessage(__con));
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //user_point_tにuserid,point,point_rateを登録する。うまくいかなかった場合、ロールバックする。
    sprintf(sendBuf, "INSERT INTO user_point_t(user_id, user_point, user_mag) VALUES(%c, %d, %f)", userid, point, point_rate); //送信データ作成
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("INSERT failed: %s", PQerrorMessage(__con));
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //user_authority_tにuserid,authを登録する。うまくいかなかった場合、ロールバックする。
    sprintf(sendBuf, "INSERT INTO user_authority_t(user_id, user_authority) VALUES(%c, %d)", userid, auth); //送信データ作成
    res = PQexec(__con, sendBuf);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("INSERT failed: %s", PQerrorMessage(__con));
        //ロールバック
        res = PQexec(__con, "ROLLBACK");
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    //登録完了を通知し、ユーザ情報を一度に表示する
    sprintf(sendBuf, "登録完了しました。%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__lsoc, sendBuf, sendLen, 0); //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
    sprintf(sendBuf, "userid:%c, phoneNum:%d, userPass:%s, userName:%s, point:%d, point_rate:%f, auth:%d%s", userid, phoneNum, userPass, userName, point, point_rate, auth, ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__lsoc, sendBuf, sendLen, 0); //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //トランザクションの終了
    res = PQexec(__con, "COMMIT");
    PQclear(res);

    return 0;
}

