CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2
TARGET = dz8
SOURCES = main.cpp Rational.cpp
HEADERS = Rational.hpp Buffer.hpp

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
