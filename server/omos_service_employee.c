#include "omos.h"

void service_employee(PGconn *__con, int __soc, int *__auth, int *__reg){
    int tel;    //電話番号
    int person; //人数
    int cnt;
    int perm_1, perm_2;
    char comm[BUFSIZE];  //コマンド
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;
    pthread_t selfId = pthread_self();

    while(1){
        if(__auth[0] == AHQ){  //本部
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"MENU\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(__auth[0] == ACOR){    //COR
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"CORRECT\"，\"MENU\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER);
        }else if(__auth[0] == AMGR){    //店長
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"REREG\"，\"REDEL\"，\"KITCHEN\"，\"TREG\"，\"MENU\"，\"DEMAND\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(__auth[0] == ACLERK){    //店員
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"REREG\"，\"REDEL\"，\"KITCHEN\"，\"TREG\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        
        recvLen = receive_message(__soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, REREG) == 0 && (__auth[0] == AMGR || __auth[0] == ACLERK)){
                reserveReg(__con, __soc);
            }else if(strcmp(comm, REDEL) == 0 && (__auth[0] == AMGR || __auth[0] == ACLERK)){
                reserveDel(__con, __soc);
            }else if(strcmp(comm, KITCHEN) == 0 && (__auth[0] == AMGR || __auth[0] == ACLERK)){
                if(kitchen() == 0){ //キッチンの端末として登録完了: 0
                    __reg[0] = 1;    //register[0] = kitchen_y_n
                    break;
                }else{
                    sprintf(sendBuf, "既に他の端末がキッチンの端末として利用されています%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, CORRECT) == 0){
                while(1){
                    sprintf(sendBuf, "CORRECTの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"CCHECK\"，\"SACHECK\"です．%s", ENTER, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(__soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        sscanf(recvBuf, "%s", comm);
                        if(strcmp(comm, CCHECK) == 0){
                            if(correctCheck() != 0){     //集計確認正常完了: 0
                                sprintf(sendBuf, "集計確認に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else if(strcmp(comm, SACHECK) == 0){
                            if(saleCheck() == 0){     //売上確認正常完了: 0
                                sprintf(sendBuf, "売上確認に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }
                    }
                }
            }else if((strcmp(comm, MENU) == 0) && (__auth[0] == AHQ || __auth[0] == ACOR || __auth[0] == AMGR || __auth[0] == ACLERK)){
                menu(__con, __soc, __auth);
            }else if((strcmp(comm, DEMAND) == 0) && (__auth[0] == AHQ || __auth[0] == AMGR)){
                demand(__con, __soc);
            }else if(strcmp(comm, STCHECK) == 0 && (__auth[0] == AHQ || __auth[0] == AMGR || __auth[0] == ACLERK)){
                if(storageCheck() == -1){
                    sprintf(sendBuf, "在庫確認に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, TREG) == 0 && (__auth[0] == AMGR || __auth[0] == ACLERK)){
                if(tableReg() == -1){
                    sprintf(sendBuf, "卓登録に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "操作を終了し，ログイン画面に戻ります%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }else{
                sprintf(sendBuf, "無効なコマンドです%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }
}