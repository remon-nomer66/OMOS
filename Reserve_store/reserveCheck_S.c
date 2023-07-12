#include "omos.h"

int reserveCheck_S(PGconn *__con, int __soc, int *__auth){

int recvLen, sendLen;　//送受信データ長
char recvBuf[BUFSIZE], sendBuf[BUFSIZE];//送受信用バッファ
char sql[BUFSIZE];
PGresult *res;　//PGresult型の変数resを宣言
int store_id;


store_id = *__auth[2]; 

//入力された店舗番号の行を一斉に表示
sprintf(sendBuf, "SELECT account_id, number_of_people, reserve_date reserve_time FROM reserve_t WHERE store_id= %d", store_id);　//送信データ作成
res = PQexec(__con, sendBuf);　//送信データを実行
//実行結果を表示
for(i = 0; i < PQntuples(res); i++){
    sprintf(sendBuf, "%s %s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3));　//送信データ作成
    sendLen = strlen(sendBuf);　//送信データ長
    send(__soc, sendBuf , sendLen, 0);　//送信
    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
    recvBuf{recvLen} = '\0';　//受信データにNULLを追加
}

prinf("%s", sendBuf);

//上手くいかなければエラーを表示する。
if(PQresultStatus(res) != PGRES_TUPLES_OK){ 
printf("店舗情報を確認できませんでした。\n");
PQclear(res);
return -1;
}

sprintf(sendBuf, "削除しますか?(y/n) %s", ENTER); //送信データ作成
sendLen = strlen(sendBuf);  //送信データ長
send(__soc, sendBuf, sendLen, 0);   //送信
recvLen = recv(__soc, recvBuf);
recvBuf{recvLen} = '\0';　//受信データにNULLを追加


if(strcmp(recvBuf,"y")==0){
    if(reserveDel_S(__con, __soc ,*auth[2]) == 0){
        sprintf(sendBuf, "メニューの変更は正常に完了しました．%s", ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        }else{
            sprintf(sendBuf,"予約確認を終了します。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(__soc, sendBuf, sendLen, 0);
        }
    }

PQclear(res);
return 0;
}
