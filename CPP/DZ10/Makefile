CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2
TARGETS = task1 task2 task3

SANITIZE_FLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror -g -O1 \
                 -fsanitize=address,undefined -fno-omit-frame-pointer

all: $(TARGETS)

task1: task1.cpp
	$(CXX) $(CXXFLAGS) task1.cpp -o task1

task2: task2.cpp
	$(CXX) $(CXXFLAGS) task2.cpp -o task2

task3: task3.cpp
	$(CXX) $(CXXFLAGS) task3.cpp -o task3

run1: task1
	./task1

run2: task2
	./task2

run3: task3
	./task3

sanitize:
	$(CXX) $(SANITIZE_FLAGS) task1.cpp -o task1_sanitize
	$(CXX) $(SANITIZE_FLAGS) task2.cpp -o task2_sanitize
	$(CXX) $(SANITIZE_FLAGS) task3.cpp -o task3_sanitize

clean:
	rm -f $(TARGETS) task1_sanitize task2_sanitize task3_sanitize

.PHONY: all run1 run2 run3 sanitize clean
