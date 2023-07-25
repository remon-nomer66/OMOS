#include "omos.h"

int tableAlt(PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int cnt;
    int tmp = 0;
    char sql[BUFSIZE];
    
    while(1){
        sprintf(sendBuf, "卓登録をする場合0、卓削除をする場合1を入力してください%s%s", ENTER, DATA_END); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加
        cnt = sscanf(recvBuf, "%d", &tmp);    //受信データをint型に変換
        
        //もしrecvBufが0だった場合tableRegを呼び出す
        if(cnt == 1 && tmp == 0){
        	tableReg(con, soc, recvBuf, sendBuf, u_info, s_info);
            sprintf(sendBuf, "0haitta%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0);   //送信
            break;
        }else if(cnt == 1 && tmp == 1){
            tableDel(selfId, con, soc, recvBuf, sendBuf, s_info);
            sprintf(sendBuf, "1haitta%s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0);   //送信
            break;
        //もしrecvBufが0でも1でもなかった場合
        }else{
            sprintf(sendBuf, "0か1を入力してください%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(soc, sendBuf, sendLen, 0);   //送信
        }

    }

    return 0;
}
