LIBS:= random.o

es2_202.exe: es2_202.cpp ${LIBS}
	g++ -o $@ $^

%.o: %.cpp %.h
	g++ -c $<

clean:
	rm *.o
