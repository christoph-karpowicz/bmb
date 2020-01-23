LIBS = -pthread -lnsl -lm -lcjson
SRC = $(shell find ./src -name '*.c')

main:
	gcc $(SRC) -o bmb $(LIBS)

dev:
	gcc -D DEV -fsanitize=undefined $(SRC) -o bmb $(LIBS)