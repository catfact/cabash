CC = gcc
CFLAGS = -std=gnu11
CFLAGS += -Wall -Wextra
# CFLAGS += -Werror
CFLAGS += -g
LIBS = -lncurses -lpanel -lpthread

SRC = ca.c main.c metro.c ui.c

all: ca

ca: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o ca $(LIBS)

clean:
	rm ca
