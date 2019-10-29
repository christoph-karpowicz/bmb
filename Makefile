LIBS = -pthread -lnsl -lm
SRC = $(shell find ./src -name '*.c')

main:
	gcc -fsanitize=undefined $(SRC) -o app $(LIBS)