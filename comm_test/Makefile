##
## omos Makefile
##
CC			= gcc
LIBS		= -lpq -lpthread
SRCS		= test_server_main.c setup_listen.c common_func.c test_service.c test_func.c
HDRS       	= test.h
OBJS		= $(SRCS:.c=.o)
PROGRAM		= test_server

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###