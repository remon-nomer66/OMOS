#include "omos.h"

int pointUse(PGconn *__con, int __soc, int __totalPrice, int *__u_info, pthread_t __selfId){

    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    int point;  //ポイント数
    int usePoint;   //使用するポイント数
    char sql[BUFSIZE];
    int flag;   //ポイント使用フラグ

    //u_infoを元に、pointをデータベースから取得
    point = pointCheck(con, soc, u_info, selfId);

    //pointを使用
    while(1){
        sendLen = sprintf(sendBuf, "現在のポイント数は%dです。何ポイント使用しますか？%s", point, ENTER);
        send(__lsoc, sendBuf, sendLen, 0);  //送信
        recvLen = recv(__lsoc, recvBuf, BUFSIZE, 0);    //受信
        recvBuf[recvLen] = '\0';    //文字列化
        usePoint = atoi(recvBuf);   //使用するポイント数
        if(usePoint > point){   //現在のポイント数が使用したいポイントを超えていた場合、エラーを返し際入力を行う
            sendLen = sprintf(sendBuf, "使用したいポイントが現在のポイント数を超えています。%s", ENTER);    //送信
            send(__lsoc, sendBuf, sendLen, 0);  //送信
        }else{
            break;  //現在のポイント数が使用したいポイントを超えていない場合、ループを抜ける
        }
    }

    //使用分のpointをdis_point()関数に送信、引数をcon,soc, point, u_info,selfIdを渡す
    flag = dis_point(con, soc, usePoint, u_info[0], selfId); 
    //もしflagが1だった場合、エラーを返し、終了
    if(flag == 1){
        sendLen = sprintf(sendBuf, "ポイント使用に失敗しました。%s", ENTER);
        send(__lsoc, sendBuf, sendLen, 0);  //送信
        return -1;
    }

    //合計金額を計算
    __totalPrice -= usePoint;

    //合計金額を送信
    sendLen = sprintf(sendBuf, "合計金額は%d円です。%s", __totalPrice, ENTER);
    send(__lsoc, sendBuf, sendLen, 0);  //送信

    return __totalPrice;
}

