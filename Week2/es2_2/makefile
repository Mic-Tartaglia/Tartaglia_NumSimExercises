LIBS:= random.o

es2_2.exe: es2_2.cpp ${LIBS}
	g++ -o $@ $^

%.o: %.cpp %.h
	g++ -c $<

clean:
	rm *.o
