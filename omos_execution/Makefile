##
## omos Makefile
##
CC					= gcc
LIBS				= -lpq -lpthread
SRCS				= $(MAIN_SRCS) $(FUNC_NAKASHIMA) $(FUNC_IZAKI) $(FUNC_UCHIDA) $(FUNC_OKA) $(FUNC_SAIKI) $(FUNC_HAMAMATSU) $(FUNC_MURAYAMA) $(FUNC_YOSHIDA)
MAIN_SRCS			:= omos_server_main.c setup_listen.c common_func.c omos_controller.c omos_service_guest.c omos_service_already.c omos_service_employee.c omos_service_menu.c omos_service_reserve.c omos_service_reserve_s.c
FUNC_NAKASHIMA		:= userReg.c userChange.c authChg.c storeChg.c demand.c history.c pay.c pointCheck.c pointUse.c evalue.c
FUNC_IZAKI			:= menuReg.c menuDel.c menuChg.c storageCheck.c
FUNC_UCHIDA			:=
FUNC_OKA			:= reserveShow.c reserveCheck.c reserveReg.c reserveChg.c reserveDel.c userCheck.c userCheckSQL.c order.c tableStoreReg.c reserveShow_s.c reserveCheck_s.c reserveReg_s.c reserveChg_s.c reserveDel_s.c
FUNC_SAIKI			:= janken.c
FUNC_HAMAMATSU		:= correct.c corectChg.c saleCheck.c
FUNC_MURAYAMA		:= kitchen.c kitchenView.c kitchenFlag.c kitchenRegDel.c
FUNC_YOSHIDA		:= tableReg.c tableDel.c
HDRS       			= $(MAIN_HDR) $(FUNC_HDR_NAKASHIMA) $(FUNC_HDR_IZAKI) $(FUNC_HDR_UCHIDA) $(FUNC_HDR_OKA) $(FUNC_HDR_SAIKI) $(FUNC_HDR_HAMAMATSU) $(FUNC_HDR_YOSHIDA)
MAIN_HDR			:= omos.h
FUNC_HDR_NAKASHIMA	:= demand.h history.h pay.h store.h
FUNC_HDR_IZAKI		:= menu.h storageCheck.h
FUNC_HDR_UCHIDA		:=
FUNC_HDR_OKA		:= reserve.h order.h
FUNC_HDR_SAIKI		:=
FUNC_HDR_HAMAMATSU	:= correct.h
FUNC_HDR_MURAYAMA	:= kitchen.h
FUNC_HDR_YOSHIDA	:= table.h
OBJS		= $(SRCS:.c=.o)
PROGRAM		= omos_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###