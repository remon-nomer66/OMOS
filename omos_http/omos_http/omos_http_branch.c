#include "omos_http.h"

int omos_http_branch(pthread_t selfId, PGconn *con, int soc, int *u_info, char *pw, int *s_info, char command[][BUFSIZE], char param[][BUFSIZE], char *http_header, char *http_body, int *body_size, int layer){
    int comm_return;
    char trimmd_comm[5][5];

    printf("comm: %s\n", command[0]);

    if(strcmp(command[0], LOGIN) == 0){
        if(http_user_check(selfId, con, soc, u_info, pw, command, param) == 0){
	        response_user_OK(http_header, http_body, u_info, pw, body_size);
            return 200;
        }else{
	        response_user_NG(http_header, http_body, u_info, pw, body_size);
            return 200;
        }
    }else if(strcmp(command[0], RESERVE_CHCK) == 0){
        comm_return = http_reserve_check(selfId, con, soc, u_info);
        if(comm_return == 0){
            if(response_reserve_check_OK(selfId, con, soc, http_header, http_body, u_info, body_size) != 0){
                return 404;
            }
            return 200;
        }else if(comm_return == 1){
	        if(response_reserve_check_NG(selfId, con, soc, http_header, http_body, u_info, body_size) != 0){
                return 404;
            }
            return 200;
        }else{
            return 404;
        }
    }else if(strcmp(command[0], STORE_INFO) == 0){
        if(http_reserve_store(selfId, con, soc, http_header, http_body, body_size) == 0){
	        return 200;
        }else{
	        return 404;
        }
    }else if(strcmp(command[0], RESERVE) == 0){
        comm_return = http_reserve_reg(selfId, con, soc, u_info, command, param);
        if(comm_return == 0){
            if(response_reserve_reg_OK(selfId, con, soc, http_header, http_body, u_info, body_size, param) != 0){
                return 404;
            }
            return 200;
        }else if(comm_return == 1){
            if(response_reserve_reg_NG(http_header, http_body, u_info, body_size) != 0){
                return 404;
            }
            return 400;
        }else{
            return 200;
        }
    }else if(strcmp(command[0], ORDER_CHCK) == 0){
        if(http_order_check(selfId, con, soc, s_info, command, param, http_body) == 0){
            response_order_check_OK(http_header, http_body, u_info, body_size);
            return 200;
        }else{
            response_order_check_NG(http_header, http_body, u_info, body_size);
            return 404;
        }
    }else if(strcmp(command[0], ORDER) == 0){
      if(http_order(selfId, con, soc, s_info, u_info, layer, command, param, http_body, trimmd_comm) == 0){
            if(response_order_OK(selfId, con, soc, param, http_header, http_body, u_info, body_size, trimmd_comm) != 0){
                response_order_NG(http_header, http_body, u_info, body_size);
                return 200;
            }
            return 200;
        }else{
            response_order_NG(http_header, http_body, u_info, body_size);
            return 200;
        }
    }
}
