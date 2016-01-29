OPTIONS=-std=c++11 -g -Wfatal-errors

tests: tests.o json.o
	c++ $(OPTIONS) -o tests tests.o json.o

tests.o: tests.cpp json.o
	c++ $(OPTIONS) -c tests.cpp

json.o: json.hpp json.cpp
	c++ $(OPTIONS) -c json.cpp

clean:
	rm tests *.o
