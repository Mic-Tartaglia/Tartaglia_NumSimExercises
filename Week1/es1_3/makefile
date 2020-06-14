LIBS:= random.o

es1_3.exe: es1_3.cpp ${LIBS}
	g++ -o $@ $^

%.o: %.cpp %.h
	g++ -c $<

clean:
	rm *.o
