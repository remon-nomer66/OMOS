#include "omos.h"

int service_employee(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info){
    int tel;    //電話番号
    int person; //人数
    int cnt;
    int perm_1, perm_2;
    char comm[BUFSIZE];  //コマンド
    int recvLen, sendLen;
    int flag = 0;

    while(1){
        if(u_info[1] == AHQ){  //本部
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"MENU\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(u_info[1] == ACOR){    //COR
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"CORRECT\"，\"MENU\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER);
        }else if(u_info[1] == AMGR){    //店長
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"REREG\"，\"REDEL\"，\"KITCHEN\"，\"TREG\"，\"MENU\"，\"DEMAND\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }else if(u_info[1] == ACLERK){    //店員
            sprintf(sendBuf, "行いたい操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"REREG\"，\"REDEL\"，\"KITCHEN\"，\"TREG\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了したい場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
        }
        sendLen = sprintf(sendBuf, "%s", DATA_END);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, REREG) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                reserveReg(con, soc);
            }else if(strcmp(comm, REDEL) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                reserveDel(con, soc);
            }else if(strcmp(comm, KITCHEN) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                if(kitchen() == 0){ //キッチンの端末として登録完了: 0
                    return 0;
                }
            }else if(strcmp(comm, CORRECT) == 0){
                while(1){
                    sprintf(sendBuf, "CORRECTの中で扱いたいコマンドを入力してください．%s利用可能なコマンドは\"CCHECK\"，\"SACHECK\"です%sCORRECTから抜ける場合は\"END\"と入力してください%s", ENTER, ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);

                    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                    recvLen = receive_message(soc, recvBuf, BUFSIZE);
                    if(recvLen > 0){
                        recvBuf[recvLen - 1] = '\0';
                        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
                        sscanf(recvBuf, "%s", comm);
                        if(strcmp(comm, CCHECK) == 0){
                            correctCheck();     //集計確認正常完了: 0
                        }else if(strcmp(comm, SACHECK) == 0){
                            saleCheck();     //売上確認正常完了: 0
                        }else if(strcmp(comm, END) == 0){
                            break;
                        }
                    }
                }
            }else if((strcmp(comm, MENU) == 0) && (u_info[1] == AHQ || u_info[1] == ACOR || u_info[1] == AMGR || u_info[1] == ACLERK)){
                menu(selfId, theradParam->con, threadParam->soc, u_info, s_info);
            }else if((strcmp(comm, DEMAND) == 0) && (u_info[1] == AHQ || u_info[1] == AMGR)){
                demand();
            }else if(strcmp(comm, STCHECK) == 0 && (u_info[1] == AHQ || u_info[1] == AMGR || u_info[1] == ACLERK)){
                storageCheck();
            }else if(strcmp(comm, TREG) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                tableReg();
            }else if(strcmp(comm, END) == 0){
                return 0;
            }
        }
    }
}