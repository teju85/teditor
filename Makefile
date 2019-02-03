
DEBUG          ?= 0
VERBOSE        ?= 0

BINDIR         := $(shell pwd)/bin
DOCDIR         := html

PCRE2_BINDIR   := $(BINDIR)/pcre2
PCRE2_LIB      := $(PCRE2_BINDIR)/lib/libpcre2-8.a
PCRE2_INCLUDE  := $(PCRE2_BINDIR)/include
PCRE2_DIR      := external/pcre2

CATCH2_DIR     := external/Catch2

TS_DIR         := external/tree-sitter
TS_BINDIR      := $(BINDIR)/tree-sitter
TS_LIB         := $(TS_BINDIR)/libts.a

SRC            := src
TESTS          := unittests
INCLUDES       := $(SRC) \
                  $(TESTS) \
                  $(PCRE2_INCLUDE) \
                  $(CATCH2_DIR)/single_include/catch2 \
                  $(TS_DIR)/src \
                  $(TS_DIR)/include \
                  $(TS_DIR)/externals/utf8proc
LIBRARIES      := $(PCRE2_LIB) $(TS_LIB)
INCS           := $(foreach inc,$(INCLUDES),-I$(inc))
CC             := gcc
CCFLAGS        := -O3 -std=c99 $(INCS)
CXX            := g++
CXXFLAGS       := -std=gnu++11 -Wall -Werror $(INCS)
LD             := g++
LDFLAGS        :=
AR             := ar
ARFLAGS        := rcs
EXE            := $(BINDIR)/teditor
CPPSRC         := $(shell find $(SRC) -name "*.cpp")
HEADERS        := $(shell find $(SRC) -name "*.h")
CORE_OBJS      := $(patsubst %.cpp,%.o,$(CPPSRC))
TESTSRC        := $(shell find $(TESTS) -name "*.cpp")
TEST_OBJS      := $(patsubst %.cpp,%.o,$(TESTSRC)) \
                  $(CORE_OBJS)
TESTEXE        := $(BINDIR)/tests
ifeq ($(DEBUG),1)
    CXXFLAGS   += -g
    LDFLAGS    += -g
    CXXFLAGS   += -DDEBUG_BUILD
else
    CXXFLAGS   += -O3
    LDFLAGS    += -O3
    CXXFLAGS   += -UDEBUG_BUILD
endif
ifeq ($(VERBOSE),1)
    PREFIX     :=
else
    PREFIX     := @
endif


default:
	@echo "make what? Available targets are:"
	@echo "  teditor     - Build the editor"
	@echo "  tests       - Build the unit-tests and run them"
	@echo "  doc         - Build doxygen documentation"
	@echo "  clean       - Clean the build files"
	@echo "  clean_all   - Clean even the build files"
	@echo "  stats       - Source code statistics"
	@echo "Flags to customize behavior:"
	@echo "  DEBUG   - Get a debug build if it is 1. Also enables debug"
	@echo "            logging in Logger class. [0]"
	@echo "  VERBOSE - Print the actual commands. [0]"

teditor: $(EXE)

$(EXE): main.o $(CORE_OBJS) $(LIBRARIES)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Building $@ ..."; \
	fi
	$(PREFIX)$(LD) $(LDFLAGS) -o $@ $^

tests: $(TESTEXE)

$(TESTEXE): $(TEST_OBJS) $(LIBRARIES)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Building $@ ..."; \
	fi
	$(PREFIX)$(LD) $(LDFLAGS) -o $@ $^
	$(TESTEXE)

$(TESTS)/%.o: $(TESTS)/%.cpp $(HEADERS)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Compiling $< ..."; \
	fi
	$(PREFIX)$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp $(HEADERS)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Compiling $< ..."; \
	fi
	$(PREFIX)$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(CORE_OBJS) main.o $(EXE)
	rm -rf $(TEST_OBJS) $(TESTEXE)

clean_all: clean pcre2-clean tree-sitter-clean
	rm -rf $(BINDIR) $(DOCDIR)


stats:
	@echo -n "SRC:   Line/Word/Char counts: "
	@find main.cpp $(SRC) -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1
	@echo -n "TESTS: Line/Word/Char counts: "
	@find $(TESTS) -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1

doc:
	rm -rf $(DOCDIR)
	doxygen Doxyfile

pcre2: $(PCRE2_LIB)

$(PCRE2_LIB):
	cd $(PCRE2_DIR) && \
	    git update-index --assume-unchanged Makefile.in aclocal.m4 configure && \
	    ./configure --prefix=$(PCRE2_BINDIR) && \
	    make -j && \
	    make install

pcre2-clean:
	cd $(PCRE2_DIR) && \
	    make clean


TS_RT_OBJ      := $(BINDIR)/runtime.o
TS_ALL_OBJS    := $(TS_RT_OBJ) \
                  $(BINDIR)/parser-c.o \
                  $(BINDIR)/parser-cpp.o \
                  $(BINDIR)/scanner-cpp.o \
                  $(BINDIR)/parser-javascript.o \
                  $(BINDIR)/parser-json.o

tree-sitter: $(TS_LIB)

$(TS_LIB): $(TS_ALL_OBJS)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Linking $@ ..."; \
	fi
	$(PREFIX)mkdir -p $(TS_BINDIR)
	$(PREFIX)$(AR) $(ARFLAGS) $@ $^

$(TS_RT_OBJ): $(TS_DIR)/src/runtime/runtime.c
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Compiling $< ..."; \
	fi
	$(PREFIX)$(CC) -c $(CCFLAGS) -o $@ $<

$(BINDIR)/parser-%.o: $(TS_DIR)-%/src/parser.c
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Compiling $< ..."; \
	fi
	$(PREFIX)$(CC) -c $(CCFLAGS) -o $@ $<

$(BINDIR)/scanner-%.o: $(TS_DIR)-%/src/scanner.cc
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Compiling $< ..."; \
	fi
	$(PREFIX)$(CXX) $(CXXFLAGS) -c -o $@ $<

tree-sitter-clean:
	rm -rf $(TS_ALL_OBJS) $(TS_LIB)
