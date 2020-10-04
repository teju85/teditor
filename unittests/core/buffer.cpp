#include "testutils.h"
#include "core/buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Buffer::Location") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
  REQUIRE(Point(0, 2) == ml.getPoint());
  REQUIRE("multiline.txt" == ml.bufferName());
  REQUIRE(4 == ml.length());
}

TEST_CASE("Buffer::KillLine") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
  REQUIRE(Point(0, 2) == ml.getPoint());
  REQUIRE("multiline.txt" == ml.bufferName());
  REQUIRE(4 == ml.length());

  SECTION("full-line") {
    auto del = ml.killLine();
    REQUIRE("for multi-line buffer!" == del);
    REQUIRE(4 == ml.length());
    REQUIRE("" == ml.at(2).get());
    ml.undo();
    REQUIRE(4 == ml.length());
    REQUIRE("for multi-line buffer!" == ml.at(2).get());
    ml.redo();
    REQUIRE(4 == ml.length());
    REQUIRE("" == ml.at(2).get());
  }

  SECTION("partial-line") {
    for(int i=0;i<4;++i) ml.right();
    auto del = ml.killLine();
    REQUIRE("multi-line buffer!" == del);
    REQUIRE(4 == ml.length());
    REQUIRE("for " == ml.at(2).get());
    ml.undo();
    REQUIRE(4 == ml.length());
    REQUIRE("for multi-line buffer!" == ml.at(2).get());
    ml.redo();
    REQUIRE(4 == ml.length());
    REQUIRE("for " == ml.at(2).get());
  }

  SECTION("line-end") {
    ml.up();
    ml.endOfLine();
    auto del = ml.killLine();
    REQUIRE("\n" == del);
    REQUIRE(3 == ml.length());
    REQUIRE("Testing123for multi-line buffer!" == ml.at(1).get());
    ml.undo();
    REQUIRE(4 == ml.length());
    REQUIRE("Testing123" == ml.at(1).get());
    REQUIRE("for multi-line buffer!" == ml.at(2).get());
    ml.redo();
    REQUIRE(3 == ml.length());
    REQUIRE("Testing123for multi-line buffer!" == ml.at(1).get());
  }
}

TEST_CASE("Buffer::BadFile") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/nofile.txt", 2);
  REQUIRE(1 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  REQUIRE("nofile.txt" == ml.bufferName());
}

TEST_CASE("Buffer::DirMode") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {100, 10}, "samples");
  REQUIRE(16 == ml.length());
  REQUIRE("samples" == ml.bufferName());
}

TEST_CASE("Buffer::Clear") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  ml.clear();
  REQUIRE(1 == ml.length());
}

TEST_CASE("Buffer::CharAt") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  REQUIRE('*' == ml.charAt({0,0}));
}

TEST_CASE("Buffer::Buffer2screen") {
  Buffer ml;
  Point start = {0, 0};
  Point dim = {30, 10};
  setupBuff(ml, start, dim, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  REQUIRE(1 == ml.totalLinesNeeded(dim));
  REQUIRE('*' == ml.charAt({0,0}));
  REQUIRE(Point(0, 0) == ml.buffer2screen({0, 0}, start, dim));
  dim = {5, 10};
  REQUIRE(2 == ml.totalLinesNeeded(dim));
  REQUIRE(Point(4, 2) == ml.buffer2screen({4, 1}, start, dim));
  REQUIRE(Point(0, 3) == ml.buffer2screen({5, 1}, start, dim));
  REQUIRE(Point(4, 5) == ml.buffer2screen({9, 2}, start, dim));
}

TEST_CASE("Window::Screen2buffer") {
  Buffer ml;
  Point start = {0, 0};
  Point dim = {30, 10};
  setupBuff(ml, start, dim, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  REQUIRE(1 == ml.totalLinesNeeded(dim));
  REQUIRE(start ==
          ml.screen2buffer(ml.buffer2screen(start, start, dim), start, dim));
  dim = {5, 10};
  REQUIRE(2 == ml.totalLinesNeeded(dim));
  REQUIRE(Point(4, 1) ==
          ml.screen2buffer(ml.buffer2screen({4, 1}, start, dim), start, dim));
  REQUIRE(Point(5, 1) ==
          ml.screen2buffer(ml.buffer2screen({5, 1}, start, dim), start, dim));
  REQUIRE(Point(9, 2) ==
          ml.screen2buffer(ml.buffer2screen({9, 2}, start, dim), start, dim));
}

TEST_CASE("Buffer::Insert") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  ml.begin();
  ml.insert('T');
  REQUIRE("T* Hello" == ml.at(0).get());
  REQUIRE("Testing123" == ml.at(1).get());

  // inserting strings
  ml.insert("AA");
  REQUIRE("TAA* Hello" == ml.at(0).get());
  REQUIRE("Testing123" == ml.at(1).get());

  // adding bunch of chars at the same time
  std::string str("line1");
  REQUIRE_NOTHROW(ml.insert(str));
  REQUIRE("TAAline1* Hello" == ml.at(0).get());
}

TEST_CASE("Buffer::RemoveRegion") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());

  Point start(0, 0), end(0, 3);
  // end includes a newline
  auto del = ml.removeRegion(start, end);
  REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif\n"
          == del);
  REQUIRE(18 == ml.length());
  // undo should now just work!
  ml.begin();
  ml.insert(del);
  REQUIRE(21 == ml.length());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("" == ml.at(3).get());

  // end doesn't include a newline
  end = {ml.at(2).length(), 2};
  del = ml.removeRegion(start, end);
  REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif"
          == del);
  REQUIRE(19 == ml.length());
  // undo should now just work!
  ml.begin();
  ml.insert(del);
  REQUIRE(21 == ml.length());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::LengthOf") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(ml.at(0).length() == ml.lengthOf(0));
}

TEST_CASE("Buffer::GotoLine") {
  Buffer ml;
  Point dim = {30, 10};
  setupBuff(ml, {0, 0}, dim, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  ml.gotoLine(10, dim);
  REQUIRE(Point(0, 10) == ml.getPoint());
  ml.right();
  ml.gotoLine(1, dim);
  REQUIRE(Point(0, 1) == ml.getPoint());
  ml.gotoLine(30, dim);
  REQUIRE(Point(0, 20) == ml.getPoint());
}

TEST_CASE("Buffer::SortRegionsEmptyLine") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  ml.startRegion();
  auto pos = ml.getRegion();
  ml.nextPara();
  auto del = ml.regionAsStr();
  REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif\n"
          == del);
  REQUIRE(Point(0, 3) == ml.getPoint());
  ml.sortRegion();
  auto after = ml.getPoint();
  ml.stopRegion();
  REQUIRE(21 == ml.length());
  REQUIRE(Point(ml.at(3).length(), 3) == ml.getPoint());
  REQUIRE("" == ml.at(0).get());
  REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
  REQUIRE("#endif" == ml.at(2).get());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(3).get());
  // undo
  ml.removeRegion(pos, after);
  ml.setPoint(pos);
  ml.insert(del);
  REQUIRE(21 == ml.length());
  REQUIRE(Point(ml.at(3).length(), 3) == ml.getPoint());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
  REQUIRE("#endif" == ml.at(2).get());
  REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::SortRegions") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  ml.startRegion();
  auto pos = ml.getRegion();
  ml.nextPara();
  // don't include the empty line!
  ml.left();
  auto del = ml.regionAsStr();
  REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif"
          == del);
  REQUIRE(Point(ml.at(2).length(), 2) == ml.getPoint());
  ml.sortRegion();
  auto after = ml.getPoint();
  ml.stopRegion();
  REQUIRE(21 == ml.length());
  REQUIRE(Point(ml.at(2).length(), 2) == ml.getPoint());
  REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(0).get());
  REQUIRE("#endif" == ml.at(1).get());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(2).get());
  // undo
  ml.removeRegion(pos, after);
  ml.setPoint(pos);
  ml.insert(del);
  REQUIRE(21 == ml.length());
  REQUIRE(Point(ml.at(2).length(), 2) == ml.getPoint());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
  REQUIRE("#endif" == ml.at(2).get());
  REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::KeepLinesNoMatches") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  parser::NFA regex("not there");
  ml.keepRemoveLines(regex, true);
  REQUIRE(1 == ml.length());
  REQUIRE(ml.isModified());
  // undo
  ml.undo();
  REQUIRE(21 == ml.length());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("" == ml.at(20).get());
  // redo
  ml.redo();
  REQUIRE(1 == ml.length());
}

TEST_CASE("Buffer::KeepLinesSomeMatches") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  parser::NFA nfa("include");
  ml.keepRemoveLines(nfa, true);
  REQUIRE(12 == ml.length());
  REQUIRE(ml.isModified());
  // undo
  ml.undo();
  REQUIRE(21 == ml.length());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("" == ml.at(20).get());
  // redo
  ml.redo();
  REQUIRE(12 == ml.length());
}

TEST_CASE("Buffer::KeepLinesAllMatches") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  parser::NFA nfa(".");  // this unfortunately removes empty lines!
  ml.keepRemoveLines(nfa, true);
  REQUIRE(3 == ml.length());
  REQUIRE(ml.isModified());
  // undo
  ml.undo();
  REQUIRE(4 == ml.length());
  REQUIRE("* Hello" == ml.at(0).get());
  // redo
  ml.redo();
  REQUIRE(3 == ml.length());
}

TEST_CASE("Buffer::RemoveLinesNoMatches") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  parser::NFA nfa("not there");
  ml.keepRemoveLines(nfa, false);
  REQUIRE(21 == ml.length());
  REQUIRE_FALSE(ml.isModified());
  // undo
  ml.undo();
  REQUIRE(21 == ml.length());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("" == ml.at(20).get());
  // redo
  ml.redo();
  REQUIRE(21 == ml.length());
}

TEST_CASE("Buffer::RemoveLinesSomeMatches") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  parser::NFA nfa("include");
  ml.keepRemoveLines(nfa, false);
  REQUIRE(9 == ml.length());
  REQUIRE(ml.isModified());
  // undo
  ml.undo();
  REQUIRE(21 == ml.length());
  REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
  REQUIRE("" == ml.at(20).get());
  // redo
  ml.redo();
  REQUIRE(9 == ml.length());
}

TEST_CASE("Buffer::RemoveLinesAllMatches") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  parser::NFA nfa(".");
  ml.keepRemoveLines(nfa, false);
  REQUIRE(1 == ml.length());
  REQUIRE(ml.isModified());
  // undo
  ml.undo();
  ///@todo: check this out (this is eating one line!)
  REQUIRE(3 == ml.length());
  REQUIRE("* Hello" == ml.at(0).get());
  // redo
  ml.redo();
  REQUIRE(1 == ml.length());
}

TEST_CASE("Buffer::MatchCurrentParen") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());
  ml.nextPara();
  ml.down();
  ml.nextWord();
  ml.right(); // '<'
  bool isOpen = ml.matchCurrentParen();
  REQUIRE(isOpen);
  REQUIRE(Point(18, 4) == ml.getPoint());
  isOpen = ml.matchCurrentParen();
  REQUIRE(!isOpen);
  REQUIRE(Point(9, 4) == ml.getPoint());
}

TEST_CASE("Buffer::Mode") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
  REQUIRE("text" == ml.modeName());
}

TEST_CASE("Buffer::Mode:setMode") {
  Buffer ml;
  REQUIRE("text" == ml.modeName());
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
  REQUIRE("text" == ml.modeName());
  ml.makeReadOnly();
  REQUIRE("ro" == ml.modeName());
  ml.setMode(Mode::createMode("c++"));
  REQUIRE("c++" == ml.modeName());
}

TEST_CASE("Buffer::TextModeIndent") {
  SECTION("indent") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 2);
    REQUIRE(Point(0, 2) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
    ml.indent();
    REQUIRE(ml.isModified());
    REQUIRE("  This should indent" == ml.at(2).get());
    REQUIRE(Point(2, 2) == ml.getPoint());
    ml.undo();
    REQUIRE("This should indent" == ml.at(2).get());
    REQUIRE(Point(0, 2) == ml.getPoint());
  }
  SECTION("indent - from middle") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 2);
    ml.right(); ml.right(); ml.right();
    REQUIRE(Point(3, 2) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
    ml.indent();
    REQUIRE(ml.isModified());
    REQUIRE("  This should indent" == ml.at(2).get());
    REQUIRE(Point(2, 2) == ml.getPoint());
    ml.undo();
    REQUIRE("This should indent" == ml.at(2).get());
    REQUIRE(Point(0, 2) == ml.getPoint());
  }
  SECTION("noindent") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 0);
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
    ml.indent();
    REQUIRE_FALSE(ml.isModified());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
  }
  SECTION("deindent") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 4);
    REQUIRE(Point(0, 4) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
    ml.indent();
    REQUIRE(ml.isModified());
    REQUIRE("This should deindent" == ml.at(4).get());
    REQUIRE(Point(0, 4) == ml.getPoint());
    ml.undo();
    REQUIRE("  This should deindent" == ml.at(4).get());
    REQUIRE(Point(2, 4) == ml.getPoint());
  }
  SECTION("deindent - from middle") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 4);
    ml.right(); ml.right(); ml.right();
    REQUIRE(Point(3, 4) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
    ml.indent();
    REQUIRE(ml.isModified());
    REQUIRE("This should deindent" == ml.at(4).get());
    REQUIRE(Point(0, 4) == ml.getPoint());
    ml.undo();
    REQUIRE("  This should deindent" == ml.at(4).get());
    REQUIRE(Point(2, 4) == ml.getPoint());
  }
}

TEST_CASE("Buffer::RegionAsStr") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
  REQUIRE(4 == ml.length());
  SECTION("newline at the beginning") {
    ml.endOfLine();
    ml.startRegion();
    ml.down();
    ml.endOfLine();
    auto str = ml.regionAsStr();
    REQUIRE("\nTesting123" == str);
    ml.stopRegion();
  }
  SECTION("full line") {
    ml.startRegion();
    ml.endOfLine();
    auto str = ml.regionAsStr();
    REQUIRE("* Hello" == str);
    ml.stopRegion();
  }
  SECTION("newline at the end") {
    ml.startRegion();
    ml.endOfLine();
    ml.right();
    auto str = ml.regionAsStr();
    REQUIRE("* Hello\n" == str);
    ml.stopRegion();
  }
  SECTION("partial lines") {
    ml.right();
    ml.right();
    ml.startRegion();
    ml.down();
    ml.endOfLine();
    ml.left();
    auto str = ml.regionAsStr();
    REQUIRE("Hello\nTesting12" == str);
    ml.stopRegion();
  }
}

TEST_CASE("Buffer::Cut") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
  REQUIRE(4 == ml.length());
  SECTION("Preceeding newline") {
    ml.endOfLine();
    ml.startRegion();
    ml.down();
    ml.endOfLine();
    auto before = ml.getPoint();
    auto regs = ml.getRegion();
    auto del = ml.removeRegion(regs, before);
    REQUIRE("\nTesting123" == del);
    REQUIRE(3 == ml.length());
  }
  SECTION("Succeeding newline") {
    ml.startOfLine();
    ml.startRegion();
    ml.down();
    auto before = ml.getPoint();
    auto regs = ml.getRegion();
    auto del = ml.removeRegion(regs, before);
    REQUIRE("* Hello\n" == del);
    REQUIRE(3 == ml.length());
  }
  SECTION("Preceeding & Succeeding newline") {
    ml.endOfLine();
    ml.startRegion();
    ml.down();
    ml.down();
    ml.startOfLine();
    auto after = ml.getPoint();
    auto regs = ml.getRegion();
    auto del = ml.removeRegion(regs, after);
    REQUIRE("\nTesting123\n" == del);
    ///@todo: issue with trailing newline now!!
    REQUIRE(3 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE("for multi-line buffer!" == ml.at(1).get());
  }
}

TEST_CASE("Buffer::cursor") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
  REQUIRE(21 == ml.length());
  REQUIRE(Point(0, 0) == ml.getPoint());

  SECTION("basics") {
    ml.down();
    REQUIRE(Point(0, 1) == ml.getPoint());
    ml.begin();
    REQUIRE(ml.hasPointOn(0));
    REQUIRE_FALSE(ml.hasPointOn(1));
    ml.down();
    REQUIRE_FALSE(ml.hasPointOn(0));
    REQUIRE(ml.hasPointOn(1));
  }

  SECTION("simpleMovements") {
    ml.endOfLine();
    REQUIRE(Point(ml.at(0).length(), 0) == ml.getPoint());
    ml.startOfLine();
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.right();
    REQUIRE(Point(1, 0) == ml.getPoint());
    ml.left();
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.down();
    REQUIRE(Point(0, 1) == ml.getPoint());
    ml.up();
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.end();
    int y = ml.length() - 1;
    int x = ml.at(y).length();
    REQUIRE(Point(x, y) == ml.getPoint());
    ml.begin();
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.pageDown(9);
    REQUIRE(Point(0, 9) == ml.getPoint());
    ml.pageUp(9);
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.nextPara();
    REQUIRE(Point(0, 3) == ml.getPoint());
    ml.previousPara();
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.nextWord();
    REQUIRE(Point(7, 0) == ml.getPoint());
    ml.previousWord();
    REQUIRE(Point(0, 0) == ml.getPoint());
  }

  SECTION("left at buffer beginning") {
    ml.left();
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.left();
    REQUIRE(Point(0, 0) == ml.getPoint());
  }

  SECTION("left at line beginning") {
    ml.down();
    ml.left();
    REQUIRE(Point(ml.at(0).length(), 0) == ml.getPoint());
  }

  SECTION("right at line end") {
    ml.endOfLine();
    ml.right();
    REQUIRE(Point(0, 1) == ml.getPoint());
  }

  SECTION("right at buffer end") {
    ml.end();
    ml.right();
    int y = ml.length() - 1;
    int x = ml.at(y).length();
    REQUIRE(Point(x, y) == ml.getPoint());
    ml.right();
    REQUIRE(Point(x, y) == ml.getPoint());
  }

  ///@todo: add more such movement tests in future!

  SECTION("save and restore") {
    REQUIRE(Point(0, 0) == ml.getPoint());
    ml.setPoint({0, 2});
    REQUIRE(Point(0, 2) == ml.getPoint());
  }
}

TEST_CASE("Buffer::SingleCursorEdits") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  ml.begin();
  REQUIRE_FALSE(ml.isModified());

  SECTION("insert char") {
    ml.insert('T');
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE(Point(1, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE(Point(1, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("insert newline") {
    ml.right(); ml.right();
    ml.insert('\n');
    REQUIRE(5 == ml.length());
    REQUIRE("* " == ml.at(0).get());
    REQUIRE("Hello" == ml.at(1).get());
    REQUIRE(Point(0, 1) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE(4 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(2, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE(5 == ml.length());
    REQUIRE("* " == ml.at(0).get());
    REQUIRE("Hello" == ml.at(1).get());
    REQUIRE(Point(0, 1) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("insert newline at the end of a line") {
    ml.endOfLine();
    ml.insert('\n');
    REQUIRE(5 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE("" == ml.at(1).get());
    REQUIRE(Point(0, 1) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE(4 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE(5 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE("" == ml.at(1).get());
    REQUIRE(Point(0, 1) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("insert newline at the beginning of a line") {
    ml.down();
    ml.insert('\n');
    REQUIRE(5 == ml.length());
    REQUIRE("" == ml.at(1).get());
    REQUIRE("Testing123" == ml.at(2).get());
    REQUIRE(Point(0, 2) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE(4 == ml.length());
    REQUIRE("Testing123" == ml.at(1).get());
    REQUIRE(Point(0, 1) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE(5 == ml.length());
    REQUIRE("" == ml.at(1).get());
    REQUIRE("Testing123" == ml.at(2).get());
    REQUIRE(Point(0, 2) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("insert string") {
    ml.insert("AA");
    REQUIRE("AA* Hello" == ml.at(0).get());
    REQUIRE(Point(2, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE("AA* Hello" == ml.at(0).get());
    REQUIRE(Point(2, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove from front") {
    ml.remove();
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
    REQUIRE_FALSE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE_FALSE(ml.redo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE_FALSE(ml.isModified());
  }

  SECTION("remove char") {
    ml.right();
    ml.remove();
    REQUIRE(" Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(1, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE(" Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove newline") {
    ml.endOfLine(); ml.right();
    REQUIRE(Point(0, 1) == ml.getPoint());
    ml.remove();
    REQUIRE(3 == ml.length());
    REQUIRE("* HelloTesting123" == ml.at(0).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE(4 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE("Testing123" == ml.at(1).get());
    REQUIRE(Point(0, 1) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE(3 == ml.length());
    REQUIRE("* HelloTesting123" == ml.at(0).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove current char") {
    ml.right();
    ml.remove(true);
    REQUIRE("*Hello" == ml.at(0).get());
    REQUIRE(Point(1, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(1, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE("*Hello" == ml.at(0).get());
    REQUIRE(Point(1, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove current newline") {
    ml.endOfLine();
    REQUIRE(Point(7, 0) == ml.getPoint());
    ml.remove(true);
    REQUIRE(3 == ml.length());
    REQUIRE("* HelloTesting123" == ml.at(0).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE(4 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE("Testing123" == ml.at(1).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE(3 == ml.length());
    REQUIRE("* HelloTesting123" == ml.at(0).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove region") {
    ml.startRegion();
    ml.right();
    ml.right();
    REQUIRE(Point(2, 0) == ml.getPoint());
    ml.remove();
    REQUIRE_FALSE(ml.isRegionActive());
    REQUIRE("Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(2, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE("Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove region with cursor coming before") {
    ml.endOfLine();
    ml.startRegion();
    ml.left();
    ml.left();
    REQUIRE(Point(5, 0) == ml.getPoint());
    ml.remove();
    REQUIRE_FALSE(ml.isRegionActive());
    REQUIRE("* Hel" == ml.at(0).get());
    REQUIRE(Point(5, 0) == ml.getPoint());
    REQUIRE(ml.undo());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(7, 0) == ml.getPoint());
    REQUIRE(ml.redo());
    REQUIRE("* Hel" == ml.at(0).get());
    REQUIRE(Point(5, 0) == ml.getPoint());
    REQUIRE(ml.isModified());
  }

  SECTION("remove region and copy") {
    ml.startRegion();
    ml.right();
    ml.right();
    REQUIRE(Point(2, 0) == ml.getPoint());
    auto del = ml.removeAndCopy();
    REQUIRE_FALSE(ml.isRegionActive());
    REQUIRE("Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE("* " == del);
    REQUIRE(ml.isModified());
  }

  SECTION("remove region and copy with cursor coming before") {
    ml.endOfLine();
    ml.startRegion();
    ml.left();
    ml.left();
    REQUIRE(Point(5, 0) == ml.getPoint());
    auto del = ml.removeAndCopy();
    REQUIRE_FALSE(ml.isRegionActive());
    REQUIRE("* Hel" == ml.at(0).get());
    REQUIRE(Point(5, 0) == ml.getPoint());
    REQUIRE("lo" == del);
    REQUIRE(ml.isModified());
  }

  SECTION("no region trying to copy") {
    auto del = ml.removeAndCopy();
    REQUIRE_FALSE(ml.isRegionActive());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Point(0, 0) == ml.getPoint());
    REQUIRE(0U == del.size());
    REQUIRE_FALSE(ml.isRegionActive());
    REQUIRE_FALSE(ml.isModified());
  }
}

TEST_CASE("Buffer::MultipleUndoRedo1") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
  REQUIRE(4 == ml.length());
  ml.begin();
  REQUIRE_FALSE(ml.isModified());
  // insert char
  ml.insert('T');
  REQUIRE("T* Hello" == ml.at(0).get());
  REQUIRE(Point(1, 0) == ml.getPoint());
  // insert newline
  ml.endOfLine();
  ml.insert('\n');
  REQUIRE(5 == ml.length());
  REQUIRE("" == ml.at(1).get());
  REQUIRE(Point(0, 1) == ml.getPoint());
  // undo1
  REQUIRE(ml.undo());
  REQUIRE(4 == ml.length());
  REQUIRE("T* Hello" == ml.at(0).get());
  REQUIRE(Point(8, 0) == ml.getPoint());
  // undo2
  REQUIRE(ml.undo());
  REQUIRE(4 == ml.length());
  REQUIRE("* Hello" == ml.at(0).get());
  REQUIRE(Point(0, 0) == ml.getPoint());
  // more undo's are useless!
  REQUIRE_FALSE(ml.undo());
  // redo's
  REQUIRE(ml.redo());
  REQUIRE(ml.redo());
  REQUIRE(5 == ml.length());
  REQUIRE("T* Hello" == ml.at(0).get());
  REQUIRE("" == ml.at(1).get());
  REQUIRE(Point(0, 1) == ml.getPoint());
  REQUIRE(ml.isModified());
  // more redo's are useless!
  REQUIRE_FALSE(ml.redo());
}

TEST_CASE("Buffers") {
  Buffers buffs;
  REQUIRE(0 == buffs.size());
  REQUIRE(buffs.empty());
  buffs.push_back(new Buffer("test1"));
  REQUIRE(!buffs.empty());
  auto list = buffs.namesList();
  REQUIRE(1 == list.size());
  REQUIRE("test1" == list[0]);
  buffs.push_back(new Buffer("test2"));
  REQUIRE(!buffs.empty());
  list = buffs.namesList();
  REQUIRE(2 == list.size());
  REQUIRE("test1" == list[0]);
  REQUIRE("test2" == list[1]);
  buffs.push_back(new Buffer("test3"));
  REQUIRE(!buffs.empty());
  list = buffs.namesList();
  REQUIRE(3 == list.size());
  REQUIRE("test1" == list[0]);
  REQUIRE("test2" == list[1]);
  REQUIRE("test3" == list[2]);
  buffs.erase(1);
  list = buffs.namesList();
  REQUIRE(2 == list.size());
  REQUIRE("test1" == list[0]);
  REQUIRE("test3" == list[1]);
  buffs.clear();
  REQUIRE(buffs.empty());
}

TEST_CASE("Buffers::uniquify") {
  Buffers buffs;
  auto* buf = buffs.push_back("scratch");
  REQUIRE("scratch" == buf->bufferName());
  REQUIRE(1 == buffs.size());
  buf = buffs.push_back("something");
  REQUIRE("something" == buf->bufferName());
  REQUIRE(2 == buffs.size());
  buf = buffs.push_back("scratch");
  REQUIRE("scratch_1" == buf->bufferName());
  REQUIRE(3 == buffs.size());
  buf = buffs.push_back("scratch");
  REQUIRE("scratch_2" == buf->bufferName());
  REQUIRE(4 == buffs.size());
  buf = buffs.push_back("scratch_4");
  REQUIRE("scratch_4" == buf->bufferName());
  REQUIRE(5 == buffs.size());
  buf = buffs.push_back("scratch");
  REQUIRE("scratch_3" == buf->bufferName());
  REQUIRE(6 == buffs.size());
  buffs.clear();
  REQUIRE(buffs.empty());
}

TEST_CASE("Buffer::MaxLine") {
  SECTION("negative") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", -1);
    REQUIRE(Point(0, 0) == ml.getPoint());
  }
  SECTION("large number") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 100);
    REQUIRE(Point(0, 3) == ml.getPoint());
  }
  SECTION("normal") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 1);
    REQUIRE(Point(0, 1) == ml.getPoint());
  }
}

} // end namespace teditor
