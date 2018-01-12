run: main.cpp algorithms.cpp
	g++ -o program main.cpp
	./program $(x) $(y) $(z)

clean:
	rm program
