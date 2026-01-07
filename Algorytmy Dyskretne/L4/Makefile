CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

all: maxflow matching

maxflow: src/maxflow.cpp src/flow_utils.hpp
	$(CXX) $(CXXFLAGS) -o maxflow src/maxflow.cpp

matching: src/matching.cpp src/flow_utils.hpp
	$(CXX) $(CXXFLAGS) -o matching src/matching.cpp

clean:
	rm -f maxflow matching
