#include "omos.h"

int correct(PGconn *__con, int __soc, int __auth){
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];    //送受信用バッファ
    int recvLen, sendLen;   //送受信データ長
    pthread_t selfId = pthread_self();  //スレッドID

    
    while(1){
        //期間の指定
        sprintf(sendBuf, "開始年月日（例：20230627）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //recvBufに数字８桁含まれていた場合
        if(!isdigit(recvBuf)){
            //取得した開始年月日を格納
            char start[13];
            strncpy(start, recvBuf, 4);
            start[4] = '-';
            strncpy(start+5, recvBuf+4, 2);
            start[7] = '-';
            strncpy(start+8, recvBuf+6, 2);
            start[10] = '\0';
            break;
        }

        //recvBufに数字以外が含まれていた場合
        else{
            sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
    }
    

    while(1){
        //期間の指定
        sprintf(sendBuf, "終了年月日（例：20230627）%s", ENTER); //送信データ作成
        sendLen = strlen(sendBuf);  //送信データ長
        send(__soc, sendBuf, sendLen, 0);   //送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
        recvBuf[recvLen] = '\0';    //受信データにNULLを追加

        //recvBufに数字８桁含まれていた場合
        if(!isdigit(recvBuf)){
            //取得した終了年月日を格納
            char end[13];
            strncpy(end, recvBuf, 4);
            end[4] = '-';
            strncpy(end+5, recvBuf+4, 2);
            end[7] = '-';
            strncpy(end+8, recvBuf+6, 2);
            end[10] = '\0';
            break;
        }

        //recvBufに数字以外が含まれていた場合
        else{
            sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
        }
    }

    //時間指定するかどうか
    sprintf(sendBuf, "時間指定しますか？（はい：1、いいえ：0）%s", ENTER); //送信データ作成
    sendLen = strlen(sendBuf);  //送信データ長
    send(__soc, sendBuf, sendLen, 0);   //送信
    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
    recvBuf[recvLen] = '\0';    //受信データにNULLを追加

    //時間指定する場合
    if(recvBuf == 1){
        //時間指定
        while(1){
            sprintf(sendBuf, "対象の開始時間を指定してください（例：2035）。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
            recvBuf[recvLen] = '\0';    //受信データにNULLを追加
            
            //recvBufの値が0000から2359の範囲の場合
            if(recvBuf >= 0000 && recvBuf <= 2359){
                //取得した対象の時間を格納
                char start_target[6];
                strncpy(start_target, recvBuf, 2);
                start_target[2] = ':';
                strncpy(start_target+3, recvBuf+2, 2);
                start_target[5] = '\0';
                break;
            }

            //recvBufに無効な範囲の数字以外が含まれていた場合
            else{
                sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(__soc, sendBuf, sendLen, 0);   //送信
            }
        }

        while(1){
            sprintf(sendBuf, "対象の終了時間を指定してください（例：2035）。%s", ENTER); //送信データ作成
            sendLen = strlen(sendBuf);  //送信データ長
            send(__soc, sendBuf, sendLen, 0);   //送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);   //受信
            recvBuf[recvLen] = '\0';    //受信データにNULLを追加
            
            //recvBufの値が0000から2359の範囲の場合
            if(recvBuf >= 0000 && recvBuf <= 2359){
                //取得した対象の時間を格納
                char end_target[6];
                strncpy(end_target, recvBuf, 2);
                end_target[2] = ':';
                strncpy(end_target+3, recvBuf+2, 2);
                end_target[5] = '\0';
                break;
            }

            //recvBufに無効な範囲の数字以外が含まれていた場合
            else{
                sprintf(sendBuf, "入力された値が不正です。%s", ENTER); //送信データ作成
                sendLen = strlen(sendBuf);  //送信データ長
                send(__soc, sendBuf, sendLen, 0);   //送信
            }
        }  
    }

    //時間指定しない場合
    else{
        //対象の時間をNULLにする
        start_target[6] = NULL;
    }



}