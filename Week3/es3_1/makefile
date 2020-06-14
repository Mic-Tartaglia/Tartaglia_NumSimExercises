LIBS:= random.o

es3_1.exe: es3_1.cpp ${LIBS}
	g++ -o $@ $^
%.o: %.cpp %.h
	g++ -c $<
clean:
	rm *.o
exec: es3_1.exe
	./es3_1.exe
