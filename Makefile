CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Werror -O2
SRC      := $(wildcard src/*.cpp)
OBJ      := $(SRC:src/%.cpp=build/%.o)

all: acaderp

acaderp: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build acaderp

.PHONY: all clean materialtest

-include $(OBJ:.o=.d)
materialtest: build/material_test
	./build/material_test

build/material_test: tests/grp79_material_test.cpp build/grp79_course_material.o | build
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^
