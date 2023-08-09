#include "omos.h"

int service_employee(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info, int *s_info, int *reg){
    int tel;    //電話番号
    int person; //人数
    int cnt;
    int perm_1, perm_2;
    char comm[BUFSIZE];  //コマンド
    int recvLen, sendLen;
    int flag = 0;

    reg[0] = reg[1] = 0;

    while(1){
        if(u_info[1] == AHQ){  //本部
            sendLen = sprintf(sendBuf, "行う操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"MENU\"，\"CORRECT\"，\"STCHECK\"，\"SCHG\"です．%s操作を終了する場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }else if(u_info[1] == ACOR){    //COR
            sendLen = sprintf(sendBuf, "行う操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"CORRECT\"，\"MENU\"です．%s操作を終了する場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }else if(u_info[1] == AMGR){    //店長
            sendLen = sprintf(sendBuf, "行う操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"RESERVE\"，\"PAY\"，\"KREG\"，\"TREG\"，\"MENU\"，\"DEMAND\"，\"CORRECT\"，\"STCHECK\"，\"TSREG\"です．%s操作を終了する場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }else if(u_info[1] == ACLERK){    //店員
            sendLen = sprintf(sendBuf, "行う操作に合わせてコマンドを実行してください．%s利用可能なコマンドは\"RESERVE\"，\"PAY\"，\"KREG\"，\"TREG\"，\"CORRECT\"，\"STCHECK\"です．%s操作を終了する場合は\"END\"と入力してください．%s", ENTER, ENTER, ENTER);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        }
        sendLen = sprintf(sendBuf, "%s", DATA_END);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
        
        recvLen = receive_message(soc, recvBuf, BUFSIZE);
        if(recvLen > 0){
            recvBuf[recvLen - 1] = '\0';
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);
            sscanf(recvBuf, "%s", comm);
            if(strcmp(comm, PAY) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                pay(selfId, con, soc, u_info, recvBuf, sendBuf);
            }else if(strcmp(comm, RESERVE) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                reserve_s(selfId, con, soc, recvBuf, sendBuf, s_info);
            }else if(strcmp(comm, KREG) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                if(kitchenReg(selfId, con, soc, recvBuf, sendBuf, u_info, reg) == 0){
                    kitchen(selfId, con, soc, recvBuf, sendBuf, s_info, u_info, reg);
                }
            }else if(strcmp(comm, CORRECT) == 0){
                correct(selfId, con, soc, u_info);
            }else if((strcmp(comm, MENU) == 0) && (u_info[1] == AHQ || u_info[1] == ACOR || u_info[1] == AMGR || u_info[1] == ACLERK)){
                service_menu(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if((strcmp(comm, DEMAND) == 0) && (u_info[1] == AHQ || u_info[1] == AMGR)){
                demand(selfId, con, soc, u_info, recvBuf, sendBuf);
            }else if(strcmp(comm, STCHECK) == 0 && (u_info[1] == AHQ || u_info[1] == AMGR || u_info[1] == ACLERK)){
                storageCheck(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if(strcmp(comm, TREG) == 0 && (u_info[1] == AMGR || u_info[1] == ACLERK)){
                tableReg(con, soc, recvBuf, sendBuf, u_info, s_info);
            }else if(strcmp(comm, ACHG) == 0){
                authChg(selfId, con, soc, u_info, recvBuf, sendBuf);
            }else if(strcmp(comm, SCHG) == 0 && u_info[1] == AHQ){
                storeChg(selfId, con, soc, u_info, recvBuf, sendBuf);
            }else if(strcmp(comm, TSREG) == 0 && u_info[1] == AMGR){
                tableStoreReg(selfId, con, soc, recvBuf, sendBuf, u_info);
            }else if(strcmp(comm, END) == 0){
                sprintf(sendBuf, "ユーザ認証画面に戻ります．%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf);
                return 0;
            }
        }
    }
}