#include "omos_http.h"

//メッセージの表示
void http_rq_re(pthread_t selfId, int soc, char *message, unsigned int size, int rsflag){
    unsigned int i;

    if(rsflag == 0){
        printf("[C_THREAD %ld] RECV=>\n", selfId);
    }else{
        printf("[C_THREAD %ld] SEND=>\n", selfId);
    }
    
    for(i = 0; i < size; i++){
        putchar(message[i]);
    }
    printf("\n");
}