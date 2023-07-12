#include "omos_http.h"

int omos_http_branch(pthread_t selfId, PGconn *con, int soc, int *u_info, int *s_info, char command[][BUFSIZE], char param[][BUFSIZE], char *http_header, char *http_body, int *body_size, int layer){
    int comm_return;

    if(strcmp(command[0], LOGIN) == 0){
        if(http_user_check(selfId, con, soc, u_info, command, param) == 0){
            response_user_OK(http_header, http_body, u_info, &body_size);
            return 301;
        }else{
            response_user_NG(http_header, http_body, u_info, &body_size);
            return 400;
        }
    }else if(strcmp(command[0], RESERVE_CHCK) == 0){
        comm_return = http_reserve_check(selfId, con, soc, u_info);
        if(comm_return == 0){
            if(response_reserve_check_OK() != 0){
                return 404;
            }
            return 200;
        }else if(comm_return == 1){
            if(response_reserve_check_NG() != 0){
                return 404;
            }
            return 400;
        }else{
            return 404;
        }
    }else if(strcmp(command[0], RESERVE) == 0){
        comm_return = http_reserve_reg();
        if(comm_return == 0){
            if(http_reserve_reg_OK() != 0){
                return 404;
            }
            return 301;
        }else if(comm_return == 1){
            if(http_reserve_reg_NG() != 0){
                return 404;
            }
            return 400;
        }else{
            return 404;
        }
    }else if(strcmp(command[0], ORDER_CHCK) == 0){
        if(http_order_check() == 0){
            response_order_check_OK();
            return 200;
        }else{
            response_order_check_NG();
            return 404;
        }
    }else if(strcmp(command[0], ORDER) == 0){
        if(http_order() == 0){
            response_order_OK();
            return 200;
        }else{
            response_order_NG();
            return 400;
        }
    }
}