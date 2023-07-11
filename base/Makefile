##
## omos Makefile
##
CC			= gcc
LIBS		= -lpq -lpthread
<<<<<<< Updated upstream
SRCS		= $(MAIN_SRCS) $(RESERVE) $(TABLE) $(MENU) $(STORE)
MAIN_SRCS	:= omos_server_main.c setup_listen.c common_func.c omos_controller.c omos_service_user_check.c omos_service_guest.c omos_service_aleady.c omos_service_employee.c
RESERVE		:= reserve.c reserveCheck.c reserceReg.c reserveDel.c reserveChg.c
TABLE		:= order.c tableAlt.c tableReg.c tableDel.c history.c
MENU		:= menu.c menuReg.c menuDel.c menuChg.c
STORE		:= demandReg.c reserveCheck_s.c reserveDel_s.c storageCheck.c correct.c saleCheck.c kitchenDel.c pay.c
HDRS       	= $(MAIN_HDR) $(RESERVE_HDR) $(TABLE_HDR) $(MENU_HDR) $(STORE_HDR)
MAIN_HDR	:= omos.h
RESERVE_HDR	:= reserve.h
TABLE_HDR	:= table.h
MENU_HDR	:= menu.h
STORE_HDR	:= store.h
=======
SRCS		= $(MAIN_SRCS)$(RESERVE)
MAIN_SRCS	= omos_server_main.c setup_listen.c common_func.c omos_service.c omos_service_user_check.c omos_service_guest.c omos_service_aleady.c omos_service_employee.c
RESERVE		= reserve.c
HDRS       	= omos.h
>>>>>>> Stashed changes
OBJS		= $(SRCS:.c=.o)
PROGRAM		= omos_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###