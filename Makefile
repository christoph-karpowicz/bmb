LIBS = -pthread -lnsl -lm -lcjson
SRC = $(shell find ./src -name '*.c')

main:
	gcc -fsanitize=undefined $(SRC) -o app $(LIBS)