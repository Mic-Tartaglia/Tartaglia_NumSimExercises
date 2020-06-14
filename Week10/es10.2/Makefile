CC = g++
CFLAGS = -Wall -O3 --std=c++11

es10.2.exe : es10.2.o random.o
	$(CC) random.o es10.2.o -o es10.2.exe
es10.2.o : es10.2.cpp es10.2.h
	$(CC) -c es10.2.cpp -o es10.2.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o es10.2.exe seed.out best.ave.out best.path.out
exec: es10.2.exe
	./es10.2.exe
