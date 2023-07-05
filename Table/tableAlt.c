#include "omos.h"
int tableAlt(PGconn *__con, int __soc, int *__u_info, int *__s_info){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID
    int cnt;
    char sql[BUFSIZE];
    
    while(1){
        sprintf(sendBuf, "卓登録をする場合0、卓削除をする場合1を入力してください%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加
        cnt = sscanf(recvBuf, "%d", &cnt);    //受信データをint型に変換
        
        //もしrecvBufが0だった場合tableRegを呼び出す
        if(cnt == 0){
            tableReg(__con, __soc, __u_info, __s_info);
            break;
        }else if(cnt == 1){
            tableDel(__con, __soc, __u_info, __s_info);
            break;
        //もしrecvBufが0でも1でもなかった場合
        }else{
            sprintf(sendBuf, "0か1を入力してください%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }

    }

    return 0;

}