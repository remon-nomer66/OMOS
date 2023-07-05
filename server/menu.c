#include "omos.h"

void menu(PGconn *__con, int __soc, int *__auth){
    char comm[BUFSIZE];  //コマンド
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;
    pthread_t selfId = pthread_self();

    while(1){
        sprintf(sendBuf, "MENUの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"MREG\"，\"MDEL\"，\"MCHG\"です．%sMENUを終了するには\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, MREG) == 0){
                if(menuReg() == 0){     //メニュー登録正常完了: 0
                    sprintf(sendBuf, "メニューの登録は正常に完了しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }else{
                    sprintf(sendBuf, "メニューの登録に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, MDEL) == 0){
                if(menuDel() == 0){     //メニュー削除正常完了: 0
                    sprintf(sendBuf, "メニューの削除は正常に完了しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }else{
                    sprintf(sendBuf, "メニューの削除に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, MCHG) == 0){
                if(menuChg() == 0){     //メニュー変更正常完了: 0
                    sprintf(sendBuf, "メニューの変更は正常に完了しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }else{
                    sprintf(sendBuf, "メニューの変更に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, END) == 0){
                break;
            }
        }
    }
    return;
}