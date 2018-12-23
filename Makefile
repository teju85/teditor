
DEBUG         ?= 0

BINDIR        := $(shell pwd)/bin

PCRE2_BINDIR  := $(BINDIR)/pcre2
PCRE2_LIB     := $(PCRE2_BINDIR)/lib/libpcre2-8.a
PCRE2_INCLUDE := $(PCRE2_BINDIR)/include

CATCH2_DIR    := external/Catch2

DOCDIR        := html
SRC           := src
TESTS         := unittests
INCLUDES      := $(SRC) $(PCRE2_INCLUDE) $(CATCH2_DIR)/single_include/catch2
LIBRARIES     := $(PCRE2_LIB)
INCS          := $(foreach inc,$(INCLUDES),-I$(inc))
CXX           := g++
CXXFLAGS      := -std=gnu++11 -Wall -Werror $(INCS)
LD            := g++
LDFLAGS       :=
EXE           := $(BINDIR)/teditor
CPPSRC        := $(shell find $(SRC) -name "*.cpp")
HEADERS       := $(shell find $(SRC) -name "*.h")
CORE_OBJS     := $(patsubst %.cpp,%.o,$(CPPSRC))
TESTSRC       := $(shell find $(TESTS) -name "*.cpp")
TEST_OBJS     := $(patsubst %.cpp,%.o,$(TESTSRC)) \
                 $(CORE_OBJS)
TESTEXE       := tests
ifeq ($(DEBUG),1)
    CXXFLAGS  += -g
    LDFLAGS   += -g
    CXXFLAGS  += -DDEBUG_BUILD
else
    CXXFLAGS  += -O3
    LDFLAGS   += -O3
    CXXFLAGS  += -UDEBUG_BUILD
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

teditor: $(EXE)

$(EXE): main.o $(CORE_OBJS) $(LIBRARIES)
	$(LD) $(LDFLAGS) -o $@ $^

$(TESTEXE): $(TEST_OBJS) $(LIBRARIES)
	$(LD) $(LDFLAGS) -o $@ $^
	$(TESTEXE)

$(TESTS)/%.o: $(TESTS)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

doc:
	rm -rf $(DOCDIR)
	doxygen Doxyfile

clean:
	rm -rf $(CORE_OBJS) main.o $(EXE)
	rm -rf $(TEST_OBJS) $(TESTEXE)

clean_all: clean
	rm -rf $(BINDIR)

stats:
	@echo -n "SRC:   Line/Word/Char counts: "
	@find main.cpp $(SRC) -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1
	@echo -n "TESTS: Line/Word/Char counts: "
	@find $(TESTS) -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1

pcre2: $(PCRE2_LIB)

$(PCRE2_LIB):
	cd external/pcre2 && \
	    git update-index --assume-unchanged Makefile.in aclocal.m4 configure && \
	    ./configure --prefix=$(PCRE2_BINDIR) && \
	    make -j && \
	    make install
