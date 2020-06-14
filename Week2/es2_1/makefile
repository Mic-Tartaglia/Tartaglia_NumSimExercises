LIBS:= random.o

es2_1.exe: es2_1.cpp ${LIBS}
	g++ -o $@ $^

%.o: %.cpp %.h
	g++ -c $<

clean:
	rm *.o
