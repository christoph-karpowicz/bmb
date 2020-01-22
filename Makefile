LIBS = -pthread -lnsl -lm -lcjson
SRC = $(shell find ./src -name '*.c')

main:
	gcc $(SRC) -o bmb $(LIBS)

dev:
	gcc -fsanitize=undefined $(SRC) -o bmb $(LIBS)