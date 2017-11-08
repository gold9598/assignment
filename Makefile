#-----------------------------------------------------------
# 
# SWE2007: Software Experiment II (Fall 2017)
#
# Skeleton code for PA #3
# October 11, 2017
#
# Jong-Won Park
# Embedded Software Laboratory
# Sungkyunkwan University
#
#-----------------------------------------------------------

CC		= gcc
CFLAGS		= -g -Wall

RM		= rm
TAR		= tar

###### fill here if you add other code/header files #####
# add .c files here
CSRCS		= swsh.c string_sw.c

# add other files here
OTHERS	= Makefile string_sw.h
#########################################################

TARGET		= swsh
OBJECTS		= $(CSRCS:.c=.o)

ifeq (tar,$(firstword $(MAKECMDGOALS)))
  TAR_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TAR_ARGS):;@:)
endif

all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

tar:
	$(TAR) zcvf $(TAR_ARGS).tar.gz $(CSRCS) $(OTHERS)

.PHONY:	clean

clean:
	$(RM) -f $(OBJECTS) $(TARGET) *~

