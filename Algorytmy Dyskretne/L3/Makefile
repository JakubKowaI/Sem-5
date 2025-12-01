CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.
SRCS = Main.cpp BinHeap.cpp RadixHeap.cpp Diala.cpp  Dijkstra.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = L3

make: $(SRCS)
	$(CXX) -o dijkstra $(SRCS)
# 	$(CXX) -o diala $(SRCS)
# 	$(CXX) -o radixheap $(SRCS)