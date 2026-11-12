CXX ?= c++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude
LDFLAGS ?=
.PHONY: all clean libraries help FORCE
.DEFAULT_GOAL := all
# Like Y_, discover every .hlypp file and embed it into the executable.
HLYPP_FILES := $(shell find std -type f -name '*.hlypp' -not -path '*/.git/*' -not -path '*/build/*' 2>/dev/null | LC_ALL=C sort)
BUILD_DIR := build
EMBEDDER := $(BUILD_DIR)/embed_hlypp
EMBEDDED_SOURCE := $(BUILD_DIR)/embedded_libraries.cpp
SOURCES := src/main.cpp src/lexer.cpp src/parser.cpp
all: ypp
help:
	@echo "YPP build targets:"
	@echo "  make              Build the YPP executable"
	@echo "  make libraries    Generate the embedded .hlypp library table"
	@echo "  make clean        Remove generated build files and executable"
$(BUILD_DIR):
	mkdir -p $@
$(EMBEDDER): tools/embed_hlypp.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<
FORCE:
$(EMBEDDED_SOURCE): FORCE $(EMBEDDER) $(HLYPP_FILES)
	$(EMBEDDER) $@.tmp $(HLYPP_FILES)
	@if test -f $@ && cmp -s $@.tmp $@; then rm -f $@.tmp; else mv $@.tmp $@; fi
libraries: $(EMBEDDED_SOURCE)
ypp: $(SOURCES) include/ypp/token.hpp include/ypp/lexer.hpp include/ypp/ast.hpp include/ypp/parser.hpp include/ypp/embedded_libraries.hpp $(EMBEDDED_SOURCE)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $(SOURCES) $(EMBEDDED_SOURCE) $(LDFLAGS)
clean:
	rm -f ypp $(EMBEDDER) $(EMBEDDED_SOURCE) $(EMBEDDED_SOURCE).tmp
