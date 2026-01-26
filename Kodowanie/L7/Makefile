CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic

.PHONY: all clean

all: koder szum dekoder sprawdz

koder: koder.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

szum: szum.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

dekoder: dekoder.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

sprawdz: sprawdz.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f koder szum dekoder sprawdz
