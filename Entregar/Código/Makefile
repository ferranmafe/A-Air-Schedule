build1: MaximumBipartiteMatching.cpp algorithms.cpp
	g++ -o program MaximumBipartiteMatching.cpp

build2: CirculationWithDemands.cpp algorithms.cpp
	g++ -o program CirculationWithDemands.cpp

run1: MaximumBipartiteMatching.cpp algorithms.cpp
	g++ -o program MaximumBipartiteMatching.cpp
	./program $(x) $(y) $(z)

run2: CirculationWithDemands.cpp algorithms.cpp
	g++ -o program CirculationWithDemands.cpp
	./program $(x) $(y) $(z)
clean:
	rm program
