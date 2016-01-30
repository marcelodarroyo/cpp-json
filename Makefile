CC=g++
OPTIONS=-std=c++11 -O3 -Wfatal-errors

tests: tests.o json.o
	$(CC) $(OPTIONS) -o tests tests.o json.o

tests.o: tests.cpp json.o
	$(CC) $(OPTIONS) -c tests.cpp

json.o: json.hpp json.cpp
	$(CC) $(OPTIONS) -c json.cpp

clean:
	rm tests *.o
