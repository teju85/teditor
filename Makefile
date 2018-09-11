
DEBUG         ?= 0

BINDIR        := $(shell pwd)/bin
PCRE2_COMMIT  := 7fdebc1bc1c5fd87a2427bae98e9729c89e62aef
PCRE2_LIB     := $(BINDIR)/lib/libpcre2-8.a
PCRE2_INCLUDE := $(BINDIR)/include

DOCDIR        := html
SRC           := src
TESTS         := tests
TESTS1        := unittests
CATCH2        := $(TESTS1)/catch.hpp
CATCH2_URL    := https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
GTEST_DIR     := $(TESTS)/googletest
INCLUDES      := $(SRC) $(GTEST_DIR) $(GTEST_DIR)/include $(PCRE2_INCLUDE)
LIBRARIES     := $(PCRE2_LIB)
INCS          := $(foreach inc,$(INCLUDES),-I$(inc))
CXX           := g++
CXXFLAGS      := -std=gnu++11 -Wall -Werror $(INCS)
LD            := g++
LDFLAGS       :=
EXE           := $(BINDIR)/bin/teditor
CPPSRC        := $(shell find $(SRC) -name "*.cpp")
HEADERS       := $(shell find $(SRC) -name "*.h")
CORE_OBJS     := $(patsubst %.cpp,%.o,$(CPPSRC))
TESTSRC       := $(shell find $(TESTS) -name "*.cpp")
TEST_CC       := $(GTEST_DIR)/src/gtest_main.cc \
                 $(GTEST_DIR)/src/gtest-all.cc
TEST_OBJS     := $(patsubst %.cpp,%.o,$(TESTSRC)) \
                 $(patsubst %.cc,%.o,$(TEST_CC)) \
                 $(CORE_OBJS)
TESTEXE       := gtests
TEST1SRC      := $(shell find $(TESTS1) -name "*.cpp")
TEST1_OBJS    := $(patsubst %.cpp,%.o,$(TEST1SRC)) \
                 $(CORE_OBJS)
TEST1EXE      := ctests
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
	@echo "  gtests      - Build the unit-tests"
	@echo "  ctests      - Build the unit-tests based on catch2"
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

$(TEST1EXE): $(TEST1_OBJS) $(LIBRARIES)
	$(LD) $(LDFLAGS) -o $@ $^
	$(TEST1EXE)

$(TESTS1)/%.o: $(TESTS1)/%.cpp $(HEADERS) $(CATCH2)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(CATCH2):
	curl -o $(CATCH2) $(CATCH2_URL)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

doc:
	rm -rf $(DOCDIR)
	doxygen Doxyfile

clean:
	rm -rf $(CORE_OBJS) main.o $(EXE)
	rm -rf $(TEST_OBJS) $(TESTEXE)
	rm -rf $(TEST1_OBJS) $(TEST1EXE)

clean_all: clean
	rm -rf $(BINDIR) pcre2 apt-cyg curl

stats:
	@echo -n "Line/Word/Char counts: "
	@find main.cpp $(SRC) -name "*.cpp" -o -name "*.h" | xargs wc -lwc | tail -n1

$(PCRE2_LIB):
	git clone https://github.com/teju85/pcre2 && \
	    cd pcre2 && \
	    git reset --hard $(PCRE2_COMMIT) && \
	    ./configure --prefix=$(BINDIR) && \
	    make -j && \
	    make install && \
	    cd .. && \
	    rm -rf pcre2
