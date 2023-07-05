##
## omos Makefile
##
CC			= gcc
LIBS		= -lpq -lpthread
SRCS		= $(MAIN_SRCS)$(RESERVE)
MAIN_SRCS	= omos_server_main.c setup_listen.c common_func.c omos_service.c omos_service_user_check.c omos_service_guest.c omos_service_aleady.c omos_service_employee.c
RESERVE		= reserve.c
HDRS       	= omos.h
OBJS		= $(SRCS:.c=.o)
PROGRAM		= omos_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###