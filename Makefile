# auto dependency generation logic here is thanks to:
#  http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

DEBUG          ?= 0
VERBOSE        ?= 0
CURL_OPTS      ?=

# NOTE: Change this to increment release version
VERSION        := 1.8.0
MKDIR_P        := mkdir -p
CURL           := curl
OS_NAME        := $(shell uname -o | sed -e 's:/:_:g')
IS_WSL         := $(shell uname -r | grep -i microsoft)
ifeq ($(IS_WSL),)
    REL_NAME   := $(OS_NAME)
else
    REL_NAME   := WSL
endif
GIT_COMMIT     := $(shell git rev-parse --short HEAD)
DIRTY          := $(shell git status -s)
ifneq ($(DIRTY),)
    DIRTY      := -dirty
else
    DIRTY      :=
endif

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

BUILD_VERSION  := $(VERSION)@@$(GIT_COMMIT)$(DIRTY)
RELEASE_ROOT   := teditor-$(VERSION)
RELEASE_DIR    := $(RELEASE_ROOT)/$(REL_NAME)
BINROOT        := bin/$(REL_NAME)
BINDIR         := $(BINROOT)/$(TYPE)
DEPDIR         := $(BINDIR)/deps
EXE            := $(BINDIR)/teditor
TESTEXE        := $(BINDIR)/teditor-tests

ifeq ($(OS_NAME),Cygwin)
    STDCXX     := gnu++14
else
    STDCXX     := c++14
endif

SRC            := src
TESTS          := unittests
MAIN           := main

CATCH2_DIR     := $(BINROOT)/Catch2
CATCH2_HEADER  := https://raw.githubusercontent.com/catchorg/Catch2/v2.x/single_include/catch2/catch.hpp

INCLUDES       := $(SRC) \
                  $(TESTS) \
                  $(CATCH2_DIR)

LIBS           :=
INCS           := $(foreach inc,$(INCLUDES),-I$(inc))
CXX            := g++
CXXFLAGS       := -std=$(STDCXX) -Wall -Werror $(INCS) -Wno-error=unused-function
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

CXXFLAGS       += -DTEDITOR_VERSION_INFO='"$(BUILD_VERSION)"'


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
	$(MKDIR_P) $(RELEASE_DIR)/Release
	cp $(BINROOT)/Release/teditor $(RELEASE_DIR)/Release
	$(MAKE) -j DEBUG=1 teditor tests
	$(MKDIR_P) $(RELEASE_DIR)/Debug
	cp $(BINROOT)/Debug/teditor $(RELEASE_DIR)/Debug

package:
	$(MAKE) doc
	rm -rf $(RELEASE_ROOT)/html
	cp -r $(BINROOT)/html $(RELEASE_ROOT)
	tar cjf $(RELEASE_ROOT).tar.gz $(RELEASE_ROOT)

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
	rm -rf $(BINROOT)/html
	$(MKDIR_P) $(BINDIR)
	sed -e "s/PROJECT_NUMBER         =/PROJECT_NUMBER         = $(BUILD_VERSION)/" \
	    -e "s:OUTPUT_DIRECTORY       =:OUTPUT_DIRECTORY       = $(BINROOT):" \
	    < Doxyfile > $(BINROOT)/Doxyfile
	doxygen -v
	doxygen $(BINROOT)/Doxyfile

$(CATCH2_DIR):
	@if [ "$(VERBOSE)" = "0" ]; then \
	    echo "Downloading $@ ..."; \
	fi
	$(PREFIX)$(MKDIR_P) $@
	$(PREFIX)$(CURL) $(CURL_QUIET) $(CURL_OPTS) -o $@/catch.hpp $(CATCH2_HEADER)

$(DEPFILES):

include $(wildcard $(DEPFILES))
