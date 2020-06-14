compile: MolDyn_NVE.cpp MolDyn_NVE.h
	g++ -o MolDyn_NVE.exe MolDyn_NVE.cpp

clean:
	rm MolDyn_NVE.exe

exec: MolDyn_NVE.exe
	./MolDyn_NVE.exe

make copy: config.final old.final
	cp config.final config.0
	cp old.final old.0
