CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lsqlite3 -largon2

OBJS = main.o database.o funcoes.o

pokedex: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o pokedex $(LIBS)

main.o: main.c pokemon.h database.h funcoes.h
	$(CC) $(CFLAGS) -c main.c

database.o: database.c database.h pokemon.h
	$(CC) $(CFLAGS) -c database.c

funcoes.o: funcoes.c funcoes.h
	$(CC) $(CFLAGS) -c funcoes.c

clean:
	rm -f *.o pokedex