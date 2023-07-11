##
## mclientEX Makefile
##
CC		= gcc
LIBS		= 
SRCS		= mclient_ex.c setup_connect.c
HDRS            = 
OBJS		= $(SRCS:.c=.o)
PROGRAM		= mclientEX

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS) $(HDRS) 
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		rm -f *.o *~ $(PROGRAM)

###
