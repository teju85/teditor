# auto dependency generation logic here is thanks to:
#  http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

DEBUG          ?= 0
VERBOSE        ?= 0
BINROOT        ?= bin
CURL_OPTS      ?=

ifeq ($(DEBUG),1)
    TYPE       := Debug
else
    TYPE       := Release
endif
ifeq ($(VERBOSE),1)
    PREFIX     :=
    CURL_QUIET :=
else
    PREFIX     := @
    CURL_QUIET := -s
endif

OS_NAME        := $(shell uname -o)
ifeq ($(OS_NAME),Cygwin)
    STDCXX     := gnu++14
else
    STDCXX     := c++14
endif

BINDIR         := $(BINROOT)/$(TYPE)
DEPDIR         := $(BINDIR)/deps
DOCDIR         := $(BINROOT)/html

SRC            := src
TESTS          := unittests
MAIN           := main

EXE            := $(BINDIR)/teditor
TESTEXE        := $(BINDIR)/teditor-tests

MKDIR_P        := mkdir -p

CURL           := curl

CATCH2_DIR     := $(BINROOT)/Catch2
CATCH2_HEADER  := https://raw.githubusercontent.com/catchorg/Catch2/v2.x/single_include/catch2/catch.hpp

INCLUDES       := $(SRC) \
                  $(TESTS) \
                  $(CATCH2_DIR)

LIBS           :=
INCS           := $(foreach inc,$(INCLUDES),-I$(inc))
CXX            := g++
CXXFLAGS       := -std=$(STDCXX) -Wall -Werror $(INCS)
LD             := g++
LDFLAGS        :=
AR             := ar
ARFLAGS        := rcs

ifeq ($(OS_NAME),GNU/Linux)
    LIBS       += -L/usr/lib64 -lpthread
    CXXFLAGS   += -Wno-unused-function
endif

CPPSRC         := $(shell find $(SRC) -name "*.cpp")
CORE_OBJS      := $(patsubst %.cpp,$(BINDIR)/%.o,$(CPPSRC))
TESTSRC        := $(shell find $(TESTS) -name "*.cpp")
TEST_OBJS      := $(patsubst %.cpp,$(BINDIR)/%.o,$(TESTSRC))
MAINSRC        := $(shell find $(MAIN) -name "*.cpp")
MAIN_OBJS      := $(patsubst %.cpp,$(BINDIR)/%.o,$(MAINSRC))

SRC_DEPS       := $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPPSRC))
TEST_DEPS      := $(patsubst %.cpp,$(DEPDIR)/%.d,$(TESTSRC))
MAIN_DEPS      := $(patsubst %.cpp,$(DEPDIR)/%.d,$(MAINSRC))
DEPFILES       := $(SRC_DEPS) $(TEST_DEPS) $(MAIN_DEPS)

ifeq ($(DEBUG),1)
    CXXFLAGS   += -g
    LDFLAGS    += -g
    CXXFLAGS   += -DDEBUG_BUILD
else
    CXXFLAGS   += -O3
    LDFLAGS    += -O3
    CXXFLAGS   += -UDEBUG_BUILD
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
	@echo "  BINROOT - Root directory where to store build files. [bin/]"

teditor: $(EXE)

$(EXE): $(MAIN_OBJS) $(CORE_OBJS)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Building  $@ ..."; \
	fi
	$(PREFIX)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

tests: $(TESTEXE)
	$(TESTEXE)

$(TEST_OBJS): $(CATCH2_DIR)

$(TESTEXE): $(CORE_OBJS) $(TEST_OBJS)
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Building  $@ ..."; \
	fi
	$(PREFIX)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BINDIR)/%.o: %.cpp $(DEPDIR)/%.d
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Compiling CXX $< ..."; \
	fi
	$(PREFIX)$(MKDIR_P) $(shell dirname $@)
	$(PREFIX)$(MKDIR_P) $(shell dirname $(DEPDIR)/$*d)
	$(PREFIX)$(CXX) -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td $(CXXFLAGS) -c -o $@ $<
	$(PREFIX)mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

clean:
	rm -rf $(CORE_OBJS) $(MAIN_OBJS) $(EXE)
	rm -rf $(TEST_OBJS) $(TESTEXE)
	rm -rf $(DEPFILES)

clean_all:
	rm -rf $(BINDIR)


stats:
	@echo -n "CORE:  Line/Word/Char counts: "
	@find $(MAIN) $(SRC)/core -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1
	@echo -n "EXT:   Line/Word/Char counts: "
	@find $(SRC)/extensions -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1
	@echo -n "TESTS: Line/Word/Char counts: "
	@find $(TESTS) -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1

doc:
	rm -rf $(DOCDIR)
	$(MKDIR_P) $(BINDIR)
	doxygen Doxyfile

$(CATCH2_DIR):
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Downloading $@ ..."; \
	fi
	$(PREFIX)$(MKDIR_P) $@
	$(PREFIX)$(CURL) $(CURL_QUIET) $(CURL_OPTS) -o $@/catch.hpp $(CATCH2_HEADER)

$(DEPFILES):

include $(wildcard $(DEPFILES))
