CC = g++
CFLAGS = -Wall -O3 --std=c++11

es10.1.exe : es10.1.o random.o
	$(CC) random.o es10.1.o -o es10.1.exe
es10.1.o : es10.1.cpp es10.1.h
	$(CC) -c es10.1.cpp -o es10.1.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o es10.1.exe seed.out best.ave.out best.path.out
exec: es10.1.exe
	./es10.1.exe
