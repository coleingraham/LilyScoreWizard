lilyscorewizard: lilyscorewizard.o
	g++ lilyscorewizard.o -o lilyscorewizard
	rm lilyscorewizard.o
	chmod u+x lilyscorewizard

lilyscorewizard.o: lilyscorewizard.cpp
	g++ -c lilyscorewizard.cpp
