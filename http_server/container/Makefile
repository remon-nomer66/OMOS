##
## omos http Makefile
##
CC			= gcc
LIBS		= -lpq -lpthread
SRCS		= $(MAIN_SRCS) $(RESPONSE) $(TABLE) $(MENU) $(STORE)
MAIN_SRCS	:= omos_http_server_main.c setup_listen.c common_func.c http_rq_re.c omos_get.c omos_post.c omos_http_controller.c omos_http_branch.c create_http_response.c
FUNC		:= http_user_check.c http_reserve_check.c http_reserve_reg.c http_order_check.c http_order.c
RESPONSE	:= response_user_OK.c response_user_NG.c response_reserve_check_OK.c response_reserve_check_NG.c response_reserve_reg_OK.c response_reserve_reg_NG.c response_order_check_OK.c response_order_check_NG.c response_order_OK.c response_order_NG.c
HDRS       	= $(MAIN_HDR) $(RESERVE_HDR) $(TABLE_HDR) $(MENU_HDR) $(STORE_HDR)
MAIN_HDR	:= omos_http.h
OBJS		= $(SRCS:.c=.o)
PROGRAM		= omos_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###