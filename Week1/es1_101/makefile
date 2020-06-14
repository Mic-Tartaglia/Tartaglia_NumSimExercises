LIBS:= random.o

es1_1.exe: es1_1.cpp ${LIBS}
	g++ -o $@ $^
%.o: %.cpp %.h
	g++ -c $<
clean:
	rm *.o
exec: es1_1.exe
	./es1_1.exe
