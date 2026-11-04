CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lsqlite3 -largon2 -lcurl -lcjson

OBJS = main.o database.o funcoes.o api.o

pokedex: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o pokedex $(LIBS)

main.o: main.c pokemon.h database.h funcoes.h api.h
	$(CC) $(CFLAGS) -c main.c

database.o: database.c database.h pokemon.h
	$(CC) $(CFLAGS) -c database.c

funcoes.o: funcoes.c funcoes.h
	$(CC) $(CFLAGS) -c funcoes.c

api.o: api.c api.h
	$(CC) $(CFLAGS) -c api.c

clean:
	rm -f *.o pokedex