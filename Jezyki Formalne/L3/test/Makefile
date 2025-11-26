# Makefile dla parsera Bison + Flex w C++

CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall

BISON = bison
BISONFLAGS = -d

FLEX = flex
FLEXFLAGS =

# Nazwa końcowego programu
TARGET = my_parser

# Pliki źródłowe
PARSER_SRC = parser.cpp
SCANNER_SRC = scanner.cpp

# Pliki generowane
PARSER_HPP = parser.hpp

# Domyślny cel
all: $(TARGET)

# Reguła kompilacji programu
$(TARGET): $(PARSER_SRC) $(SCANNER_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Generowanie parsera z Bison
parser.cpp parser.hpp: parser.yy
	$(BISON) $(BISONFLAGS) -o parser.cpp parser.yy

# Generowanie skanera z Flex
scanner.cpp: scanner.l parser.hpp
	$(FLEX) -o scanner.cpp scanner.l

# Czyszczenie plików tymczasowych i programu
clean:
	rm -f $(TARGET) parser.cpp parser.hpp scanner.cpp

.PHONY: all clean
