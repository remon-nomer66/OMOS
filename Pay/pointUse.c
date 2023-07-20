#include "omos.h"

int pointUse(pthread_t selfId, PGconn *con, int soc, int totalPrice, int *u_info, char *recvBuf, char *sendBuf){
    int recvLen, sendLen;   //送受信データ長
    int point;  //ポイント数
    int usePoint;   //使用するポイント数
    char sql[BUFSIZE];
    int flag;   //ポイント使用フラグ

    //u_infoを元に、pointをデータベースから取得
    //point = pointCheck(con, soc, u_info, selfId);
    point = 1000;

    //pointを使用
    while(1){
        sendLen = sprintf(sendBuf, "現在のポイント数は%dです。何ポイント使用しますか？%s%s", point, ENTER, DATA_END);
        send(soc, sendBuf, sendLen, 0);  //送信
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);    //受信
        recvBuf[recvLen-1] = '\0';    //文字列化
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
        usePoint = atoi(recvBuf);   //使用するポイント数
        if(usePoint > point){   //現在のポイント数が使用したいポイントを超えていた場合、エラーを返し際入力を行う
            sendLen = sprintf(sendBuf, "使用したいポイントが現在のポイント数を超えています。%s", ENTER);    //送信
            send(soc, sendBuf, sendLen, 0);  //送信
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }else{
            break;  //現在のポイント数が使用したいポイントを超えていない場合、ループを抜ける
        }
    }

    //使用分のpointをdis_point()関数に送信、引数をcon,soc, point, u_info,selfIdを渡す
    //flag = dis_point(con, soc, usePoint, u_info[0], selfId); 
    flag = 0;
    //もしflagが1だった場合、エラーを返し、終了(エラーはdis_pointの方で吐いてもらう)

    //合計金額を計算
    totalPrice -= usePoint;
    printf("totalPrice = %d\n", totalPrice);

    return totalPrice;
}