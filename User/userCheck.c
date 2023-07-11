#include "OMOS.h"

int userCheck(PGconn *__con, int __soc){
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

    //user_tテーブルからSELECTで同一の電話番号を検索
    sprintf(sendBuf, "SELECT * FROM user_t WHERE phone_num = %d", phoneNum);
    res = PQexec(__con, sendBuf);

    //検索結果が1件の場合、パスワードを入力してもらう
    if(PQntuples(res) == 1){
        while(1){
            sprintf(sendBuf, "パスワードを入力してください。%s", ENTER); //送信データ作成
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
    }else{
        sprintf(sendBuf, "電話番号が不正です。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
    }

    //user_tテーブルからSELECTで同一の電話番号を検索
    sprintf(sendBuf, "SELECT * FROM user_t WHERE phone_num = %d AND user_pass = %s", phoneNum, userPass);
    res = PQexec(__con, sendBuf);

    //検索結果が1件の場合、user_tから氏名(user_name)を取得
    if(PQntuples(res) == 1){
        userName = PQgetvalue(res, 0, 2);  //氏名を取得
    }else{
        sprintf(sendBuf, "パスワードが不正です。%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__lsoc, sendBuf, sendLen, 0); //送信
    }

    //トランザクション終了
    res = PQexec(__con, "COMMIT");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("COMMIT failed: %s", PQerrorMessage(__con));
        PQclear(res);
        PQfinish(__con);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(__soc, sendBuf, sendLen, 0);
    }

    return 0;

}