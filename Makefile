CC = gcc
CFLAGS = -std=gnu11
CFLAGS += -Wall -Wextra
# CFLAGS += -Werror
CFLAGS += -g

INC = -I/usr/include/lua5.1
LIBS = -lncurses -lpanel -lpthread -llo -llua5.1

SRC = ca.c conf.c main.c metro.c ui.c patterns.c

all: ca

ca: $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o ca $(LIBS)

clean:
	rm ca
