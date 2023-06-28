#include "omos.h"

int computerhand, playerhand;
char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
int recvLen, sendLen;                    // 送受信データ長
pthread_t selfId = pthread_self();       // スレッドID

while（1）
{
    // お客さんにじゃんけんタイムを知らせ,入力もらう
    sprintf(sendBuf,"じゃんけんタイム！\n
    グーなら0,チョキなら1,パーなら２を押してください.\n
    オリオンじゃんけんじゃんけんぽい！\n%s"ENTER); //送信データ
    sendLen = strlen(sendBuf); //送信データ長
    send(__lsoc,sendBuf,sendLen,0);//送信
    recvLen = recv(__lsoc,recvBuf,BUFSIZE,0);//受信
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    playerhand = atoi[recvBuf];//文字列から数値に直した

    if(playerhand=0 || playerhand=1 || playerhand=2){//もし,グー,チョキ,パー以外だったらもう一回選んでもらう
        break;
    }else{}
}

send((unsigned int)time(NULL)); // 乱数の初期化
computerhand = rand() % 3 + 1;  // ランダムに画面にじゃんけんを表示する

// お客さんとコンピュータ側が出した手を表示する
sprintf(sendBuf, "あなたが出した手；%s", ENTER); // お客さんが出した手
if (playerhand == 0)
{
    sprintf(sendBuf, "グー\n%s", ENTER);
}
else if (playerhand == 1)
{
    sprintf(sendBuf, "チョキ\n%s", ENTER);
}
else if (playerhand == 2)
{
    sprintf(sendBuf, "パー\n%s", ENTER);
}

sprintf(sendBuf, "相手が出した手；%s", ENTER);
if (computerhand == 0)
{
    sprintf(sendBuf, "グー\n%s", ENTER);
}
else if (computerhand == 1)
{
    sprintf(sendBuf, "チョキ\n%s", ENTER);
}
else if (computerhand == 2)
{
    sprintf(sendBuf, "パー\n%s", ENTER);
}

if (playerhand == computerhand) // あいこの場合
{
    sprintf(sendBuf, "あいこだったの,500ポイント付与します\n%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(__lsoc, sendBuf, sendLen, 0); // 送信
    return 500;
}
else if (playerhand == 0 && computerhand == 1 || playerhand == 1 && computerhand == 2 || playerhand == 2 && computerhand == 0)
{ // 勝ちの場合

    sprintf(sendBuf, "勝ちだったので,1000ポイント付与します\n%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(__lsoc, sendBuf, sendLen, 0); // 送信
    return 1000;
}
else
{//負けの場合
    sprintf(sendBuf, "負けだったので,300ポイント付与します\n%s", ENTER);
    sendLen = strlen(sendBuf);         // 送信データ長
    send(__lsoc, sendBuf, sendLen, 0); // 送信
    return 300;
}

