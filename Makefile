# auto dependency generation logic here is thanks to:
#  http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

DEBUG          ?= 0
VERBOSE        ?= 0
BINROOT        ?= bin
CURL_OPTS      ?=

OS_NAME        := $(shell uname -o | sed -e 's/\//_/g')
# NOTE: Change this to increment release version
VERSION        := 1.8.0
RELEASE_ROOT   := teditor-$(VERSION)
RELEASE_DIR    := $(RELEASE_ROOT)/$(OS_NAME)

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

ifeq ($(OS_NAME),GNU_Linux)
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
    CXXFLAGS   += -g -DDEBUG_BUILD
    LDFLAGS    += -g
else
    CXXFLAGS   += -O3 -UDEBUG_BUILD
    LDFLAGS    += -O3
endif

CXXFLAGS       += -DTEDITOR_VERSION_INFO='"$(VERSION)"'


default:
	@echo "make what? Available targets are:"
	@echo "  doc         - Build doxygen documentation"
	@echo "  clean       - Clean the build files"
	@echo "  clean_all   - Clean even the build files"
	@echo "  release     - Prepare release package"
	@echo "  package     - After preparing release packages for all"
	@echo "                supported platforms, tarball this folder."
	@echo "  stats       - Source code statistics"
	@echo "  teditor     - Build the editor"
	@echo "  tests       - Build the unit-tests and run them"
	@echo "Flags to customize behavior:"
	@echo "  DEBUG   - Get a debug build if it is 1. Also enables debug"
	@echo "            logging in Logger class. [0]"
	@echo "  VERBOSE - Print the actual commands. [0]"
	@echo "  BINROOT - Root directory where to store build files. [bin/]"

release:
	$(MKDIR_P) $(RELEASE_DIR)
	$(MAKE) clean_all
	$(MAKE) -j teditor tests
	$(MAKE) -j DEBUG=1 teditor tests
	$(MAKE) doc
	$(MKDIR_P) $(RELEASE_DIR)/Release $(RELEASE_DIR)/Debug
	cp $(BINROOT)/Release/teditor $(RELEASE_DIR)/Release
	cp $(BINROOT)/Debug/teditor $(RELEASE_DIR)/Debug
	rm -rf $(RELEASE_ROOT)/docs
	cp -r $(BINROOT)/html/html $(RELEASE_ROOT)/docs
	rm -rf $(BINROOT)/html

package:
	tar xjf $(RELEASE_ROOT).tar.gz $(RELEASE_ROOT)

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
	rm -rf $(BINROOT)


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
	sed -e "s/PROJECT_NUMBER         =/PROJECT_NUMBER         = $(VERSION)/" < Doxyfile > $(BINROOT)/Doxyfile
	doxygen $(BINROOT)/Doxyfile

$(CATCH2_DIR):
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Downloading $@ ..."; \
	fi
	$(PREFIX)$(MKDIR_P) $@
	$(PREFIX)$(CURL) $(CURL_QUIET) $(CURL_OPTS) -o $@/catch.hpp $(CATCH2_HEADER)

$(DEPFILES):

include $(wildcard $(DEPFILES))
