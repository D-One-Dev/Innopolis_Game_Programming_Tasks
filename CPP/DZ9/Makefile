CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2
TARGET = dz9
SOURCES = main.cpp Shape.cpp String.cpp
HEADERS = Shape.hpp String.hpp

SANITIZE_FLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror -g -O1 \
                 -fsanitize=address,undefined -fno-omit-frame-pointer

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET)

sanitize:
	$(CXX) $(SANITIZE_FLAGS) $(SOURCES) -o $(TARGET)_sanitize
	./$(TARGET)_sanitize

clean:
	rm -f $(TARGET) $(TARGET)_sanitize

.PHONY: run test sanitize clean
