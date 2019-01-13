#include "testutils.h"
#include "core/buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Buffer::Location") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
    REQUIRE(Pos2di(0, 2) == ml.saveCursors()[0]);
    REQUIRE("multiline.txt" == ml.bufferName());
    REQUIRE(4 == ml.length());
}

TEST_CASE("Buffer::KillLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
    REQUIRE(Pos2di(0, 2) == ml.saveCursors()[0]);
    REQUIRE("multiline.txt" == ml.bufferName());
    REQUIRE(4 == ml.length());
    auto del = ml.killLine();
    REQUIRE(4 == ml.length());
    REQUIRE(1U == del.size());
    REQUIRE("for multi-line buffer!" == del[0]);
    del = ml.killLine();
    REQUIRE(3 == ml.length());
    REQUIRE(1U == del.size());
    REQUIRE("\n" == del[0]);
}

TEST_CASE("Buffer::BadFile") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/nofile.txt", 2);
    REQUIRE(1 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    REQUIRE("nofile.txt" == ml.bufferName());
}

TEST_CASE("Buffer::DirMode") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {100, 10}, "samples");
    REQUIRE(10 == ml.length());
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
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    REQUIRE(1 == ml.totalLinesNeeded());
    REQUIRE('*' == ml.charAt({0,0}));
    REQUIRE(Pos2di(0, 0) == ml.buffer2screen({0, 0}));
    ml.resize({0, 0}, {5, 10});
    REQUIRE(2 == ml.totalLinesNeeded());
    REQUIRE(Pos2di(4, 2) == ml.buffer2screen({4, 1}));
    REQUIRE(Pos2di(0, 3) == ml.buffer2screen({5, 1}));
    REQUIRE(Pos2di(4, 5) == ml.buffer2screen({9, 2}));
}

TEST_CASE("Buffer::Screen2buffer") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    REQUIRE(1 == ml.totalLinesNeeded());
    REQUIRE(Pos2di(0, 0) == ml.screen2buffer(ml.buffer2screen({0, 0})));
    ml.resize({0, 0}, {5, 10});
    REQUIRE(Pos2di(4, 1) == ml.screen2buffer(ml.buffer2screen({4, 1})));
    REQUIRE(Pos2di(5, 1) == ml.screen2buffer(ml.buffer2screen({5, 1})));
    REQUIRE(Pos2di(9, 2) == ml.screen2buffer(ml.buffer2screen({9, 2})));
}

TEST_CASE("Buffer::Insert") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    ml.begin();
    ml.addCursorFromBack({0, 1});
    ml.insert('T');
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE("TTesting123" == ml.at(1).get());
    REQUIRE(2 == ml.cursorCount());

    // inserting strings at multiple-cursors
    ml.insert("AA");
    REQUIRE("TAA* Hello" == ml.at(0).get());
    REQUIRE("TAATesting123" == ml.at(1).get());

    ml.clearAllCursorsButFirst();
    REQUIRE(1 == ml.cursorCount());

    // adding bunch of chars at the same time
    std::vector<std::string> strs;
    strs.push_back("line1");
    REQUIRE_NOTHROW(ml.insert(strs));
    REQUIRE("TAAline1* Hello" == ml.at(0).get());
}

TEST_CASE("Buffer::RemoveRegion") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);

    Positions start, end;
    start.push_back({0, 0});
    end.push_back({0, 3});
    // end includes a newline
    auto del = ml.remove(start, end);
    REQUIRE(1U == del.size());
    REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif\n"
            == del[0]);
    REQUIRE(18 == ml.length());
    // undo should now just work!
    ml.begin();
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("" == ml.at(3).get());

    // end doesn't include a newline
    end[0] = {ml.at(2).length(), 2};
    del = ml.remove(start, end);
    REQUIRE(1U == del.size());
    REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif"
            == del[0]);
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

TEST_CASE("Buffer::Remove") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);

    // remove at the beginning of buffer
    auto del = ml.remove();
    REQUIRE(1U == del.size());
    REQUIRE("" == del[0]);
    REQUIRE(21 == ml.length());
    REQUIRE(19 == ml.at(0).length());
    // undo on empty remove case
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE(19 == ml.at(0).length());

    // remove inside a line
    ml.begin();
    ml.right();
    del = ml.remove();
    REQUIRE(1U == del.size());
    REQUIRE("#" == del[0]);
    REQUIRE(18 == ml.at(0).length());
    // undo
    ml.insert(del);
    REQUIRE(19 == ml.at(0).length());

    // remove at line beginning
    ml.begin();
    ml.down();
    del = ml.remove();
    REQUIRE(1U == del.size());
    REQUIRE("\n" == del[0]);
    REQUIRE(20 == ml.length());
    REQUIRE(Pos2di(19, 0) == ml.saveCursors()[0]);
    REQUIRE(66 == ml.at(0).length());
    // undo
    ml.insert(del);
    REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
    REQUIRE(21 == ml.length());
    REQUIRE(19 == ml.at(0).length());
    REQUIRE(47 == ml.at(1).length());
}

TEST_CASE("Buffer::RemoveCurrent") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);

    // remove at the end of buffer
    ml.end();
    auto del = ml.removeCurrent();
    REQUIRE(1U == del.size());
    REQUIRE("" == del[0]);
    REQUIRE(21 == ml.length());
    REQUIRE(0 == ml.at(ml.length()-1).length());
    // undo on empty remove case
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE(0 == ml.at(ml.length()-1).length());

    // remove inside a line
    ml.begin();
    ml.right();
    del = ml.removeCurrent();
    REQUIRE(1U == del.size());
    REQUIRE("i" == del[0]);
    REQUIRE(18 == ml.at(0).length());
    // undo
    ml.insert(del);
    REQUIRE(19 == ml.at(0).length());

    // remove at line end
    ml.begin();
    ml.endOfLine();
    del = ml.removeCurrent();
    REQUIRE(1U == del.size());
    REQUIRE("\n" == del[0]);
    REQUIRE(20 == ml.length());
    REQUIRE(Pos2di(19, 0) == ml.saveCursors()[0]);
    REQUIRE(66 == ml.at(0).length());
    // undo (it would certainly make the cursor move to next position!)
    ml.insert(del);
    REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
    REQUIRE(21 == ml.length());
    REQUIRE(19 == ml.at(0).length());
    REQUIRE(47 == ml.at(1).length());
}

TEST_CASE("Buffer::GotoLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    ml.gotoLine(10);
    REQUIRE(Pos2di(0, 10) == ml.saveCursors()[0]);
    ml.gotoLine(30);
    REQUIRE(Pos2di(0, 20) == ml.saveCursors()[0]);
}

TEST_CASE("Buffer::SortRegionsEmptyLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    ml.enableRegions();
    auto pos = ml.getRegionLocs();
    ml.nextPara();
    auto del = ml.regionAsStr();
    REQUIRE(1U == del.size());
    REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif\n"
            == del[0]);
    REQUIRE(Pos2di(0, 3) == ml.saveCursors()[0]);
    ml.sortRegions();
    auto after = ml.saveCursors();
    ml.disableRegions();
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(ml.at(3).length(), 3) == ml.saveCursors()[0]);
    REQUIRE("" == ml.at(0).get());
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
    REQUIRE("#endif" == ml.at(2).get());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(3).get());
    // undo
    ml.remove(pos, after);
    ml.restoreCursors(pos);
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(ml.at(3).length(), 3) == ml.saveCursors()[0]);
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
    REQUIRE("#endif" == ml.at(2).get());
    REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::SortRegions") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    ml.enableRegions();
    auto pos = ml.getRegionLocs();
    ml.nextPara();
    // don't include the empty line!
    ml.left();
    auto del = ml.regionAsStr();
    REQUIRE(1U == del.size());
    REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif"
            == del[0]);
    REQUIRE(Pos2di(ml.at(2).length(), 2) == ml.saveCursors()[0]);
    ml.sortRegions();
    auto after = ml.saveCursors();
    ml.disableRegions();
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(ml.at(2).length(), 2) == ml.saveCursors()[0]);
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(0).get());
    REQUIRE("#endif" == ml.at(1).get());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(2).get());
    // undo
    ml.remove(pos, after);
    ml.restoreCursors(pos);
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(ml.at(2).length(), 2) == ml.saveCursors()[0]);
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
    REQUIRE("#endif" == ml.at(2).get());
    REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::KeepLinesNoMatches") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    Pcre pc("not there");
    auto res = ml.keepRemoveLines(pc, true);
    REQUIRE(21U == res.size());
    REQUIRE("#ifndef _GNU_SOURCE" == res[0].str);
    REQUIRE(Pos2di(0, 0) == res[0].pos);
    REQUIRE("" == res[20].str);
    // pos is 0,0 because of sequential removal of lines!
    REQUIRE(Pos2di(0, 0) == res[20].pos);
    REQUIRE(1 == ml.length());

    // undo
    ml.addLines(res);
    // yea, in case of full removal, undo causes an extra line at the end!
    REQUIRE(22 == ml.length());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("" == ml.at(21).get());
}

TEST_CASE("Buffer::KeepLinesSomeMatches") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    Pcre pc("include");
    auto res = ml.keepRemoveLines(pc, true);
    REQUIRE(9U == res.size());
    REQUIRE("#ifndef _GNU_SOURCE" == res[0].str);
    REQUIRE(Pos2di(0, 0) == res[0].pos);
    REQUIRE("" == res[8].str);
    REQUIRE(Pos2di(0, 12) == res[8].pos);
    REQUIRE(12 == ml.length());

    // undo
    ml.addLines(res);
    REQUIRE(21 == ml.length());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("" == ml.at(20).get());
}

TEST_CASE("Buffer::RemoveLinesNoMatches") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    Pcre pc("not there");
    auto res = ml.keepRemoveLines(pc, false);
    REQUIRE(0U == res.size());
    REQUIRE(res.empty());
    REQUIRE(21 == ml.length());

    // undo
    ml.addLines(res);
    REQUIRE(21 == ml.length());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("" == ml.at(20).get());
}

TEST_CASE("Buffer::RemoveLinesSomeMatches") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    Pcre pc("include");
    auto res = ml.keepRemoveLines(pc, false);
    REQUIRE(12U == res.size());
    REQUIRE("#include <stdint.h>" == res[0].str);
    REQUIRE(Pos2di(0, 4) == res[0].pos);
    REQUIRE("#include <algorithm>" == res[11].str);
    REQUIRE(Pos2di(0, 4) == res[11].pos);
    REQUIRE(9 == ml.length());

    // undo
    ml.addLines(res);
    REQUIRE(21 == ml.length());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("" == ml.at(20).get());
}

TEST_CASE("Buffer::MatchCurrentParen") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    ml.nextPara();
    ml.down();
    ml.nextWord();
    ml.right(); // '<'
    ml.matchCurrentParen();
    REQUIRE(Pos2di(18, 4) == ml.saveCursors()[0]);
    ml.matchCurrentParen();
    REQUIRE(Pos2di(9, 4) == ml.saveCursors()[0]);
}

TEST_CASE("Buffer::Mode") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
    REQUIRE("text" == ml.modeName());
}

TEST_CASE("Buffer::TextModeIndent") {
    SECTION("indent") {
        Buffer ml;
        setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 2);
        ml.indent();
        REQUIRE("  This should indent" == ml.at(2).get());
    }
    SECTION("deindent") {
        Buffer ml;
        setupBuff(ml, {0, 0}, {30, 10}, "samples/indent.txt", 4);
        ml.indent();
        REQUIRE("This should deindent" == ml.at(4).get());
    }
}

TEST_CASE("Buffer::RegionAsStr") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
    REQUIRE(4 == ml.length());
    SECTION("newline at the beginning") {
        ml.endOfLine();
        ml.enableRegions();
        ml.down();
        ml.endOfLine();
        auto strs = ml.regionAsStr();
        REQUIRE(1U == strs.size());
        REQUIRE("\nTesting123" == strs[0]);
        ml.disableRegions();
    }
    SECTION("full line") {
        ml.enableRegions();
        ml.endOfLine();
        auto strs = ml.regionAsStr();
        REQUIRE(1U == strs.size());
        REQUIRE("* Hello" == strs[0]);
        ml.disableRegions();
    }
    SECTION("newline at the end") {
        ml.enableRegions();
        ml.endOfLine();
        ml.right();
        auto strs = ml.regionAsStr();
        REQUIRE(1U == strs.size());
        REQUIRE("* Hello\n" == strs[0]);
        ml.disableRegions();
    }
    SECTION("partial lines") {
        ml.right();
        ml.right();
        ml.enableRegions();
        ml.down();
        ml.endOfLine();
        ml.left();
        auto strs = ml.regionAsStr();
        REQUIRE(1U == strs.size());
        REQUIRE("Hello\nTesting12" == strs[0]);
        ml.disableRegions();
    }
}

TEST_CASE("Buffer::Cut") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
    REQUIRE(4 == ml.length());
    SECTION("Preceeding newline") {
        ml.endOfLine();
        ml.enableRegions();
        ml.down();
        ml.endOfLine();
        auto before = ml.saveCursors();
        auto regs = ml.getRegionLocs();
        auto del = ml.remove(regs, before);
        REQUIRE(1U == del.size());
        REQUIRE("\nTesting123" == del[0]);
        REQUIRE(3 == ml.length());
    }
    SECTION("Succeeding newline") {
        ml.startOfLine();
        ml.enableRegions();
        ml.down();
        auto before = ml.saveCursors();
        auto regs = ml.getRegionLocs();
        auto del = ml.remove(regs, before);
        REQUIRE(1U == del.size());
        REQUIRE("* Hello\n" == del[0]);
        REQUIRE(3 == ml.length());
    }
    SECTION("Preceeding & Succeeding newline") {
        ml.endOfLine();
        ml.enableRegions();
        ml.down();
        ml.down();
        ml.startOfLine();
        auto before = ml.saveCursors();
        auto regs = ml.getRegionLocs();
        auto del = ml.remove(regs, before);
        REQUIRE(1U == del.size());
        REQUIRE("\nTesting123\n" == del[0]);
        ///@todo: issue with trailing newline now!!
        REQUIRE(3 == ml.length());
    }
}

TEST_CASE("Buffer::cursor") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    REQUIRE(1 == ml.cursorCount());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);

    SECTION("basics") {
        ml.clearAllCursorsButFirst();
        REQUIRE(1 == ml.cursorCount());
        ml.addCursorFromBack({0, 1});
        REQUIRE(2 == ml.cursorCount());
        ml.addCursorFromBack({0, 1});
        REQUIRE(2 == ml.cursorCount());
        ml.down();
        ml.addCursorFromFront({0, 0});
        REQUIRE(3 == ml.cursorCount());
        Positions expected({{0, 0}, {0, 1}, {0, 2}});
        REQUIRE(expected == ml.saveCursors());
        ml.begin();
        REQUIRE(1 == ml.cursorCount());
        ml.addCursorFromBack({0, 1});
        ml.addCursorFromBack({0, 2});
        REQUIRE(3 == ml.cursorCount());
        ml.clearAllCursorsButFirst();
        REQUIRE(1 == ml.cursorCount());
        ml.begin();
        REQUIRE(ml.hasCursorOn(0));
        REQUIRE_FALSE(ml.hasCursorOn(1));
        ml.down();
        REQUIRE_FALSE(ml.hasCursorOn(0));
        REQUIRE(ml.hasCursorOn(1));
    }

    SECTION("simpleMovements") {
        ml.endOfLine();
        REQUIRE(Pos2di(ml.at(0).length(), 0) == ml.saveCursors()[0]);
        ml.startOfLine();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.right();
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        ml.left();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.down();
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        ml.up();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.end();
        int y = ml.length() - 1;
        int x = ml.at(y).length();
        REQUIRE(Pos2di(x, y) == ml.saveCursors()[0]);
        ml.begin();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.pageDown(1.f);
        REQUIRE(Pos2di(0, 9) == ml.saveCursors()[0]);
        ml.pageUp(1.f);
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.nextPara();
        REQUIRE(Pos2di(0, 3) == ml.saveCursors()[0]);
        ml.previousPara();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.nextWord();
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        ml.previousWord();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    }

    SECTION("left at buffer beginning") {
        ml.left();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.left();
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    }

    SECTION("left at line beginning") {
        ml.down();
        ml.left();
        REQUIRE(Pos2di(ml.at(0).length(), 0) == ml.saveCursors()[0]);
    }

    SECTION("right at line end") {
        ml.endOfLine();
        ml.right();
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
    }

    SECTION("right at buffer end") {
        ml.end();
        ml.right();
        int y = ml.length() - 1;
        int x = ml.at(y).length();
        REQUIRE(Pos2di(x, y) == ml.saveCursors()[0]);
        ml.right();
        REQUIRE(Pos2di(x, y) == ml.saveCursors()[0]);
    }

    ///@todo: add more such movement tests in future!

    SECTION("save and restore") {
        ml.addCursorFromBack({0, 1});
        ml.addCursorFromBack({0, 2});
        REQUIRE(3 == ml.cursorCount());
        Positions expected({{0, 0}, {0, 1}, {0, 2}});
        REQUIRE(expected == ml.saveCursors());
        Positions empty;
        ml.restoreCursors(empty);
        REQUIRE(3 == ml.cursorCount());
        Positions smaller({{0, 0}, {0, 2}});
        ml.restoreCursors(smaller);
        REQUIRE((int)smaller.size() == ml.cursorCount());
        Positions larger({{0, 0}, {0, 2}, {0, 3}, {0, 4}});
        ml.restoreCursors(larger);
        REQUIRE((int)larger.size() == ml.cursorCount());
    }
}

TEST_CASE("Buffer::SingleCursorEdits") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    ml.begin();
    REQUIRE(1 == ml.cursorCount());
    REQUIRE_FALSE(ml.isModified());

    SECTION("insert char") {
        ml.insert('T');
        REQUIRE("T* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE("T* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("insert newline") {
        ml.right(); ml.right();
        ml.insert('\n');
        REQUIRE(5 == ml.length());
        REQUIRE("* " == ml.at(0).get());
        REQUIRE("Hello" == ml.at(1).get());
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE(4 == ml.length());
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE(5 == ml.length());
        REQUIRE("* " == ml.at(0).get());
        REQUIRE("Hello" == ml.at(1).get());
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("insert newline at the end of a line") {
        ml.endOfLine();
        ml.insert('\n');
        REQUIRE(5 == ml.length());
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE("" == ml.at(1).get());
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE(4 == ml.length());
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE(5 == ml.length());
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE("" == ml.at(1).get());
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("insert newline at the beginning of a line") {
        ml.down();
        ml.insert('\n');
        REQUIRE(5 == ml.length());
        REQUIRE("" == ml.at(1).get());
        REQUIRE("Testing123" == ml.at(2).get());
        REQUIRE(Pos2di(0, 2) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE(4 == ml.length());
        REQUIRE("Testing123" == ml.at(1).get());
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE(5 == ml.length());
        REQUIRE("" == ml.at(1).get());
        REQUIRE("Testing123" == ml.at(2).get());
        REQUIRE(Pos2di(0, 2) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("insert string") {
        ml.insert("AA");
        REQUIRE("AA* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE("AA* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("remove from front") {
        ml._remove();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        REQUIRE_FALSE(ml.isModified());
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        REQUIRE_FALSE(ml.isModified());
    }

    SECTION("remove char") {
        ml.right();
        ml._remove();
        REQUIRE(" Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE(" Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("remove newline") {
        ml.endOfLine(); ml.right();
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        ml._remove();
        REQUIRE(3 == ml.length());
        REQUIRE("* HelloTesting123" == ml.at(0).get());
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE(4 == ml.length());
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE("Testing123" == ml.at(1).get());
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE(3 == ml.length());
        REQUIRE("* HelloTesting123" == ml.at(0).get());
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("remove current char") {
        ml.right();
        ml._remove(true);
        REQUIRE("*Hello" == ml.at(0).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE("*Hello" == ml.at(0).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("remove current newline") {
        ml.endOfLine();
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        ml._remove(true);
        REQUIRE(3 == ml.length());
        REQUIRE("* HelloTesting123" == ml.at(0).get());
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE(4 == ml.length());
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE("Testing123" == ml.at(1).get());
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE(3 == ml.length());
        REQUIRE("* HelloTesting123" == ml.at(0).get());
        REQUIRE(Pos2di(7, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }

    SECTION("remove region") {
        ml.enableRegions();
        ml.right();
        ml.right();
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        ml._remove();
        REQUIRE_FALSE(ml.isRegionActive());
        REQUIRE("Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        ml.redo();
        REQUIRE("Hello" == ml.at(0).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        REQUIRE(ml.isModified());
    }
}

TEST_CASE("Buffer::MultipleCursorEdits") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    ml.begin();
    ml.addCursorFromBack({0, 1});
    REQUIRE(2 == ml.cursorCount());
    REQUIRE_FALSE(ml.isModified());

    SECTION("insert char") {
        ml.insert('T');
        REQUIRE("T* Hello" == ml.at(0).get());
        REQUIRE("TTesting123" == ml.at(1).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        REQUIRE(Pos2di(1, 1) == ml.saveCursors()[1]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE("Testing123" == ml.at(1).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[1]);
        ml.redo();
        REQUIRE("T* Hello" == ml.at(0).get());
        REQUIRE("TTesting123" == ml.at(1).get());
        REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
        REQUIRE(Pos2di(1, 1) == ml.saveCursors()[1]);
        REQUIRE(2 == ml.cursorCount());
        REQUIRE(ml.isModified());
    }

    SECTION("insert string") {
        ml.insert("AA");
        REQUIRE("AA* Hello" == ml.at(0).get());
        REQUIRE("AATesting123" == ml.at(1).get());
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        REQUIRE(Pos2di(2, 1) == ml.saveCursors()[1]);
        ml.undo();
        REQUIRE("* Hello" == ml.at(0).get());
        REQUIRE("Testing123" == ml.at(1).get());
        REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
        REQUIRE(Pos2di(0, 1) == ml.saveCursors()[1]);
        ml.redo();
        REQUIRE("AA* Hello" == ml.at(0).get());
        REQUIRE("AATesting123" == ml.at(1).get());
        REQUIRE(Pos2di(2, 0) == ml.saveCursors()[0]);
        REQUIRE(Pos2di(2, 1) == ml.saveCursors()[1]);
        REQUIRE(2 == ml.cursorCount());
        REQUIRE(ml.isModified());
    }

    ///@todo: add tests for deletion operations
}

TEST_CASE("Buffer::MultipleUndoRedo1") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    ml.begin();
    REQUIRE(1 == ml.cursorCount());
    REQUIRE_FALSE(ml.isModified());
    // insert char
    ml.insert('T');
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE(Pos2di(1, 0) == ml.saveCursors()[0]);
    // insert newline
    ml.endOfLine();
    ml.insert('\n');
    REQUIRE(5 == ml.length());
    REQUIRE("" == ml.at(1).get());
    REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
    // undo1
    ml.undo();
    REQUIRE(4 == ml.length());
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE(Pos2di(8, 0) == ml.saveCursors()[0]);
    // undo2
    ml.undo();
    REQUIRE(4 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE(Pos2di(0, 0) == ml.saveCursors()[0]);
    // redo's
    ml.redo();
    ml.redo();
    REQUIRE(5 == ml.length());
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE("" == ml.at(1).get());
    REQUIRE(Pos2di(0, 1) == ml.saveCursors()[0]);
    REQUIRE(ml.isModified());
}

} // end namespace teditor
