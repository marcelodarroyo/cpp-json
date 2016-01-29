OPTIONS=-std=c++11 -g -Wfatal-errors

tests: tests.o
	c++ $(OPTIONS) -o tests tests.o json-parser.o

tests.o: tests.cpp json-parser.hpp json-parser.cpp
	c++ $(OPTIONS) -c tests.cpp json-parser.cpp

clean:
	rm tests *.o
