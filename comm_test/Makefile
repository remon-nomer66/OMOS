##
## test Makefile
##
CC		= gcc
LIBS		= -lpq -lpthread
SRCS		= $(MAIN_SRC) $(FUNC_TEST)
MAIN_SRC	:= test_server_main.c setup_listen.c common_func.c test_controller.c
FUNC_TEST	:= test_func.c
HDRS    	= $(MAIN_HDR) $(FUNC_HDR_TEST)
MAIN_HDR	:= omos.h
FUNC_HDR_TEST	:= test.h
OBJS		= $(SRCS:.c=.o)
PROGRAM		= test_s

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###
