#include "omos.h"

#define REWIND 1

void service_employee(PGconn *__soc, int __auth, int __register[]){
    int tel;    //電話番号
    int person; //人数
    char comm[BUFSIZE];  //コマンド
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int flag = 0;
    pthread_t selfId = pthread_self();

    while(1){
        if(__auth == AHQ){  //本部
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"MENU\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(__auth == ACOR){    //COR
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"CORRECT\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(__auth == AMGR){    //店長
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%S利用可能なコマンドは\"REREG\"，\"REDEL\"，\"KITCHEN\"，\"TREG\"，\"MENU\"，\"DEMAND\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(__auth == ACLERK){    //店員
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"REREG\"，\"REDEL\"，\"KITCHEN\"，\"TREG\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }
        sendLen = strlen(sendBuf);
        send(__soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, REREG) == 0 && (__auth == AMGR || __auth == ACLERK)){
                while(1){
                    sprintf(sendBuf, "予約を登録する会員の電話番号，日時，人数を\"03012345678 2023-06-18 5\"のように空白を挿入した形で入力してください%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        cnt = sscanf(recvBuf, "%d %s %d", tel, comm, person);
                        if(cnt == 3){
                            if(reserveReg() == 0){ //予約登録正常完了: 0
                                sprintf(sendBuf, "予約の登録は正常に完了しました%s", ENTER);
                                break;
                            }else{
                                sprintf(sendBuf, "予約が埋まっています%s", ENTER);
                            }
                        }else{
                            sprintf(sendBuf, "引数に誤りがあります%s", ENTER);
                        }
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }
            }else if(strcmp(comm, REDEL) == 0 && (__auth == AMGR || __auth == ACLERK)){
                while(1){
                    sprintf(sendBuf, "予約を削除する会員の電話番号，日時を\"03012345678 2023-06-18\"のように空白を挿入した形で入力してください%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        cnt = sscanf(recvBuf, "%d %s", tel, comm);
                        if(cnt == 2){
                            if(reserveReg() == 0){ //予約削除正常完了: 0
                                sprintf(sendBuf, "予約の削除は正常に完了しました%s", ENTER);
                            }else{
                                sprintf(sendBuf, "予約は存在ませんでした%s", ENTER);
                            }
                        }else{
                            sprintf(sendBuf, "引数に誤りがあります%s", ENTER);
                        }
                        sendLen = strlen(sendBuf);
                        send(__soc, sendBuf, sendLen, 0);
                        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    }
                }
            }else if(strcmp(comm, KITCHEN) == 0 && (__auth == AMGR || __auth == ACLERK)){
                if(kitchen() == 0){ //キッチンの端末として登録完了: 0
                    register[0] = 1;    //register[0] = kitchen_y_n
                    break;
                }else{
                    sprintf(sendBuf, "既に他の端末がキッチンの端末として利用されています%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else if(strcmp(comm, TREG) == 0 && (__auth == AMGR || __auth == ACLERK)){
                while(1){
                    sprintf(sendBuf, "この端末の卓に座れる人数を入力してください%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        cnt = sscanf(recvBuf, "%d", perm_1);
                        if(cnt == 1){
                            if((register[1] = tableReg()) != -1){ //卓登録: 戻り値: 卓番号，register[1] = table_num
                                break;
                            }else{
                                sprintf(sendBuf, "これ以上卓を登録できません%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                            }
                        }
                    }
                }
            }else if(strcmp(comm, CORRECT)){
                while(1){
                    sprintf(sendBuf, "CORRECTの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"CCHECK\"，\"SACHECK\"です．%s", ENTER, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
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
                        }else if(strcmp(comm, MDEL) == 0){
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
            }else if((strcmp(comm, MENU) == 0) && (__auth == AMGR || __auth == ACLERK)){
                while(1){
                    sprintf(sendBuf, "MENUの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"MREG\"，\"MDEL\"，\"MCHG\"です．%s", ENTER, ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
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
                                break;
                            }else{
                                sprintf(sendBuf, "メニューの登録に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else if(strcmp(comm, MDEL) == 0){
                            if(menuDel() == 0){     //メニュー削除正常完了: 0
                                sprintf(sendBuf, "メニューの削除は正常に完了しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }else{
                                sprintf(sendBuf, "メニューの削除に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }else if(strcmp(comm, MCHG) == 0){
                            if(menuReg() == 0){     //メニュー変更正常完了: 0
                                sprintf(sendBuf, "メニューの変更は正常に完了しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }else{
                                sprintf(sendBuf, "メニューの変更に失敗しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                break;
                            }
                        }
                    }
                }
            }else if((strcmp(comm, DEMAND) == 0) && (__auth == AHQ || __auth == AMGR)){
                while(1){
                    sprintf(sendBuf, "発注する商品の番号と個数を\"1001 200\"のように空白を挿入した形で入力してください%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        cnt = sscanf(recvBuf, "%d %d", perm_1, perm_2);
                        if(cnt == 2){
                            if(demand() == 0){      //発注正常完了: 0
                                sprintf(sendBuf, "発注は正常に完了しました．%s", ENTER);
                                sendLen = strlen(sendBuf);
                                send(__soc, sendBuf, sendLen, 0);
                                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                while(1){
                                    sprintf(sendBuf, "続けて発注する場合は\"YES\"，発注を終了する場合は\"NO\"を入力してください%s", ENTER);
                                    sendLen = strlen(sendBuf);
                                    send(__soc, sendBuf, sendLen, 0);
                                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                                    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
                                    if(recvLen > 0){
                                        recvBuf[recvLen - 1] = '\0';
                                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                                        sscanf(recvBuf, "%s", comm);
                                        if(strcmp(comm, YES) == 0){
                                            break;
                                        }else if(strcmp(comm, NO) == 0){
                                            flag = REWIND;
                                            break;
                                        }
                                    }
                                }
                                if(flag == REWIND){
                                    break;  //発注から抜ける
                                }
                            }
                        }else{
                            sprintf(sendBuf, "引数に誤りがあります%s", ENTER);
                            sendLen = strlen(sendBuf);
                            send(__soc, sendBuf, sendLen, 0);
                            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                        }
                    }
                }
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "操作を終了し，ログイン画面に戻ります%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                break;
            }else if(strcmp(comm, STCHECK) == 0 && (__auth == AHQ || __auth == AMGR || __auth == ACLERK)){
                if(storageCheck() != -1){
                    sprintf(sendBuf, "在庫確認に失敗しました．%s", ENTER);
                    sendLen = strlen(sendBuf);
                    send(__soc, sendBuf, sendLen, 0);
                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                }
            }else{
                sprintf(sendBuf, "無効なコマンドです%s", ENTER);
                sendLen = strlen(sendBuf);
                send(__soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
            }
        }
    }
}