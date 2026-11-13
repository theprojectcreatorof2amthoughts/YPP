CC ?= cc
CXX ?= c++
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude -Isrc
LDFLAGS ?=
.PHONY: all clean libraries help test-diagnostics FORCE
.DEFAULT_GOAL := all
HLYPP_FILES := $(shell find std -type f -name '*.hlypp' -not -path '*/.git/*' -not -path '*/build/*' 2>/dev/null | LC_ALL=C sort)
BUILD_DIR := build
EMBEDDER := $(BUILD_DIR)/embed_hlypp
EMBEDDED_SOURCE := $(BUILD_DIR)/embedded_libraries.c
C_OBJECTS := $(BUILD_DIR)/runtime.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/file_io.o $(BUILD_DIR)/embedded_libraries.o
CPP_OBJECTS := $(BUILD_DIR)/main.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/parser.o
all: ylang++ gy++
help:
	@echo "YLANG++ build targets:"
	@echo "  make              Build YLANG++ and the gy++ native compiler"
	@echo "  make gy++         Build the YPP native compiler driver"\n	@echo "  make libraries    Generate the embedded .hlypp library table"
	@echo "  make clean        Remove generated files and executable"
$(BUILD_DIR):
	mkdir -p $@
$(EMBEDDER): tools/embed_hlypp.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<
FORCE:
$(EMBEDDED_SOURCE): FORCE $(EMBEDDER) $(HLYPP_FILES) src/embedded_libraries.h
	$(EMBEDDER) $@.tmp $(HLYPP_FILES)
	@if test -f $@ && cmp -s $@.tmp $@; then rm -f $@.tmp; else mv $@.tmp $@; fi
libraries: $(EMBEDDED_SOURCE)
$(BUILD_DIR)/runtime.o: src/runtime.c src/runtime.h include/ypp/bridge.h src/memory.h src/file_io.h src/embedded_libraries.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Iinclude -Isrc -c $< -o $@
$(BUILD_DIR)/memory.o: src/memory.c src/memory.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Isrc -c $< -o $@
$(BUILD_DIR)/file_io.o: src/file_io.c src/file_io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Isrc -c $< -o $@
$(BUILD_DIR)/embedded_libraries.o: $(EMBEDDED_SOURCE) src/embedded_libraries.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Isrc -c $(EMBEDDED_SOURCE) -o $@
$(BUILD_DIR)/main.o: src/main.cpp src/runtime.h include/ypp/bridge.h src/embedded_libraries.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
$(BUILD_DIR)/lexer.o: src/lexer.cpp include/ypp/lexer.hpp include/ypp/token.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
$(BUILD_DIR)/parser.o: src/parser.cpp include/ypp/parser.hpp include/ypp/ast.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
ylang++: $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) -o $@ $(C_OBJECTS) $(CPP_OBJECTS) -lm $(LDFLAGS)
clean:
	rm -rf $(BUILD_DIR) ylang++ gy++ ypp

# YPP native compiler: embeds .ycpp source and links it with the YPP runtime.
GYPP_ROOT := $(CURDIR)
gy++: tools/gypp.c $(EMBEDDED_SOURCE) src/runtime.c src/memory.c src/file_io.c src/runtime.h include/ypp/bridge.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -DGYPP_PROJECT_ROOT='"$(GYPP_ROOT)"' -DGYPP_EMBEDDED_SOURCE='"$(CURDIR)/$(EMBEDDED_SOURCE)"' -o '$@' tools/gypp.c src/runtime.c src/memory.c src/file_io.c $(EMBEDDED_SOURCE) -lm

test-diagnostics: gy++
	@sh tests/test_multi_error.sh
