CC = g++
CFLAGS = -Wall -O3 --std=c++11

es8.2.exe : es8.2.o random.o
	$(CC) random.o es8.2.o -o es8.2.exe
es8.2.o : es8.2.cpp
	$(CC) -c es8.2.cpp -o es8.2.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.out *.o es8.2.exe seed.out
finder : finder.o random.o
	$(CC) random.o finder.o -o finder.exe
finder.o : min.finder.cpp
	$(CC) -c min.finder.cpp -o finder.o $(CFLAGS)
exec: es8.2.exe
	./es8.2.exe
find: finder.exe
	./finder.exe
