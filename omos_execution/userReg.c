#include "omos.h"

int userReg(pthread_t selfId, PGconn *con, int soc, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    char phoneNum[BUFSIZE];  //電話番号
    char userPass[BUFSIZE];  //パスワード
    char userName[BUFSIZE];  //氏名
    int point;  //ポイント
    double point_rate;  //ポイントレート
    int auth;  //権限情報

    //トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }

    while(1){
        //電話番号を入力してもらう
        memset(recvBuf, 0, sizeof(recvBuf));
        sprintf(sendBuf, "電話番号を入力してください(09024681234)。%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        
        //入力が数字11桁の場合
        int is_valid = 1;
        for(int i=0; i<strlen(recvBuf); i++){
            if(!isdigit(recvBuf[i])){
                is_valid = 0;
                break;
            }
        }
        if( strlen(recvBuf) == 11 && is_valid ){
            strcpy(phoneNum, recvBuf);
            break;
        }else{
            sprintf(sendBuf, "電話番号が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
    }

    while(1){
        //パスワードを入力してもらう
        memset(recvBuf, 0, sizeof(recvBuf));
        sprintf(sendBuf, "パスワードを入力してください。%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        //入力が8文字以上16文字以下の場合、パスワードとして扱う
        if( 8 <= strlen(recvBuf) && strlen(recvBuf) <= 16 ){
            //recvBufの文字列をuserPassに文字列のまま代入
            strcpy(userPass, recvBuf);
            printf("userPass: %s\n", userPass);
            break;
        }else{
            sprintf(sendBuf, "パスワードが不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
    }

    while(1){
        //氏名を入力してもらう
        memset(recvBuf, 0, sizeof(recvBuf));
        sprintf(sendBuf, "氏名を入力してください。%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0); //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0';  //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

        //入力が30文字以下の場合、氏名として扱う
        if( strlen(recvBuf) <= 30 ){
            strcpy(userName, recvBuf);
            printf("userName: %s\n", userName);
            break;
        }else{
            sprintf(sendBuf, "氏名は30文字を超えないようにしてください。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
        }
    }

    //じゃんけんを行い、勝ったら1000pt,あいこは500pt,負けは300ptを付与する
    point = janken(selfId, soc, recvBuf, sendBuf);
    point_rate = 1.0;

    //権限情報authを1にする
    auth = 1;

    //user_tにuserid,phoneNum,userName,userPassを登録する。うまくいかなかった場合、ロールバックする。
    sprintf(sendBuf, "INSERT INTO user_t(user_id, user_phone, user_name, user_pass) VALUES(nextval('user_seq'), '%s', '%s', '%s')", phoneNum, userName, userPass); //送信データ作成
    res = PQexec(con, sendBuf);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("dame:");
        printf("INSERT failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "error occured%s%s", ENTER,DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }

    //user_tのuser_idを取得する
    sprintf(sendBuf, "SELECT user_id FROM user_t WHERE user_phone = %s", phoneNum); //送信データ作成
    res = PQexec(con, sendBuf);
    printf("sendBuf: %s\n", sendBuf);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        printf("SELECT failed: %s", PQerrorMessage(con));
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "error occured%s%s", ENTER,DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }
    char  *user_id_str = PQgetvalue(res, 0, 0);
    int user_id = atoi(user_id_str);
    printf("user_id: %d\n", user_id);

    //user_point_tにuserid,point,point_rateを登録する。うまくいかなかった場合、ロールバックする。
    sprintf(sendBuf, "INSERT INTO user_point_t(user_id, user_point, user_mag) VALUES(%d, %d, %f)", user_id, point, point_rate); //送信データ作成
    res = PQexec(con, sendBuf);
    printf("sendBuf: %s\n", sendBuf);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("INSerrrrr");
        printf("INSERT failed: %s", PQerrorMessage(con));
        //ロールバック
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        PQfinish(con);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);
    }

    if(user_id == 1001){
        sprintf(sendBuf, "INSERT INTO user_authority_t(user_id, user_authority) VALUES(%d, 2)", user_id); //送信データ作成
        res = PQexec(con, sendBuf);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("INSERT failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            PQfinish(con);
            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
        }
    }else{
        sprintf(sendBuf, "INSERT INTO user_authority_t(user_id, user_authority) VALUES(%d, %d)", user_id, auth); //送信データ作成
        res = PQexec(con, sendBuf);
        if(PQresultStatus(res) != PGRES_COMMAND_OK){
            printf("INSERT failed: %s", PQerrorMessage(con));
            //ロールバック
            res = PQexec(con, "ROLLBACK");
            PQclear(res);
            PQfinish(con);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
        }
    }

    //登録完了を通知し、ユーザ情報を一度に表示する
    sprintf(sendBuf, "登録完了しました。%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(soc, sendBuf, sendLen, 0); //送信
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);

    //トランザクションの終了
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;

}