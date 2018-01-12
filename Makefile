run: MaximumBipartiteMatching.cpp algorithms.cpp
	g++ -o program MaximumBipartiteMatching.cpp
	./program $(arg1) $(arg2) $(arg3)

clean:
	rm program
