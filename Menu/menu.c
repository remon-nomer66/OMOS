#include "omos.h"

int menu(PGconn *__con, int __soc, int *__auth){
    while (1){
        char comm[BUFSIZE];  //コマンド
        char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
        int recvLen, sendLen;
        pthread_t selfId = pthread_self();

        sprintf(sendBuf, "MENUの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"MREG\"，\"MDEL\"，\"MCHG\"です．%s", ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if (recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            sscanf(recvBuf, "%s", comm);
            if (strcmp(comm, MREG) == 0){
                if (menuReg(__con, __soc, __auth) == 0){ // メニュー登録正常完了: 0
                    sprintf(sendBuf, "メニューの登録は正常に完了しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    break;
                }else{
                    sprintf(sendBuf, "メニューの登録に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    break;
                }
            }else if (strcmp(comm, MDEL) == 0){
                if (menuDel(__con, __soc, __auth) == 0){ // メニュー削除正常完了: 0
                    sprintf(sendBuf, "メニューの削除は正常に完了しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    break;
                }else{
                    sprintf(sendBuf, "メニューの削除に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    break;
                }
            }else if (strcmp(comm, MCHG) == 0){
                if (menuChg(__con, __soc, __auth) == 0){ // メニュー変更正常完了: 0
                    sprintf(sendBuf, "メニューの変更は正常に完了しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    break;
                }else{
                    sprintf(sendBuf, "メニューの変更に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    break;
                }
            }
        }
    }
}