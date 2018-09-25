#include "testutils.h"
#include "buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Buffer::Location") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    REQUIRE(0 == pos.x);
    REQUIRE(2 == pos.y);
    REQUIRE("multiline.txt" == ml.bufferName());
    REQUIRE(4 == ml.length());
}

TEST_CASE("Buffer::KillLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    REQUIRE(Pos2d<int>(0, 2) == pos);
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
    const auto& pos = ml.getCursor().at(0);
    REQUIRE(1 == ml.length());
    REQUIRE(0 == pos.x);
    REQUIRE(0 == pos.y);
    REQUIRE("nofile.txt" == ml.bufferName());
}

TEST_CASE("Buffer::DirMode") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {100, 10}, "samples");
    REQUIRE(8 == ml.length());
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
    Pos2d<int> loc = {0, 0};
    Pos2d<int> bloc = ml.buffer2screen(loc);
    REQUIRE(0 == bloc.x);
    REQUIRE(0 == bloc.y);
    loc = {4, 1};
    ml.resize({0, 0}, {5, 10});
    REQUIRE(2 == ml.totalLinesNeeded());
    bloc = ml.buffer2screen(loc);
    REQUIRE(4 == bloc.x);
    REQUIRE(2 == bloc.y);
    loc = {5, 1};
    bloc = ml.buffer2screen(loc);
    REQUIRE(0 == bloc.x);
    REQUIRE(3 == bloc.y);
    loc = {9, 2};
    bloc = ml.buffer2screen(loc);
    REQUIRE(4 == bloc.x);
    REQUIRE(5 == bloc.y);
}

TEST_CASE("Buffer::Screen2buffer") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    REQUIRE(1 == ml.totalLinesNeeded());
    Pos2d<int> loc = {0, 0};
    auto loc1 = ml.screen2buffer(ml.buffer2screen(loc));
    CHECK(loc1 == loc);
    loc = {4, 1};
    ml.resize({0, 0}, {5, 10});
    loc1 = ml.screen2buffer(ml.buffer2screen(loc));
    CHECK(loc1 == loc);
    loc = {5, 1};
    loc1 = ml.screen2buffer(ml.buffer2screen(loc));
    CHECK(loc1 == loc);
    loc = {9, 2};
    loc1 = ml.screen2buffer(ml.buffer2screen(loc));
    CHECK(loc1 == loc);
}

TEST_CASE("Buffer::Insert") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    Cursor& cu = ml.getCursor();
    cu.reset(&ml);
    cu.addBack(0, 1);
    ml.insert('T');
    REQUIRE("T* Hello" == ml.at(0).get());
    REQUIRE("TTesting123" == ml.at(1).get());
    REQUIRE(2 == cu.count());

    // inserting strings at multiple-cursors
    ml.insert("AA");
    REQUIRE("TAA* Hello" == ml.at(0).get());
    REQUIRE("TAATesting123" == ml.at(1).get());

    cu.clearAllButFirst();
    REQUIRE(1 == cu.count());

    // adding bunch of chars at the same time
    std::vector<std::string> strs;
    strs.push_back("line1");
    REQUIRE_NOTHROW(ml.insert(strs));
    REQUIRE("TAAline1* Hello" == ml.at(0).get());

    // mismatch in number of cursors
    strs.push_back("line2");
    REQUIRE_THROWS(ml.insert(strs));
}

TEST_CASE("Buffer::InsertLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    auto& cu = ml.getCursor();
    // enter somewhere inside the line
    cu.at(0) = {0, 1};
    ml.insert('\n');
    REQUIRE(5 == ml.length());
    REQUIRE("" == ml.at(1).get());
    REQUIRE("Testing123" == ml.at(2).get());
    // enter at the end of line
    cu.at(0) = {ml.at(0).length(), 0};
    ml.insert('\n');
    REQUIRE(6 == ml.length());
    REQUIRE("* Hello" == ml.at(0).get());
    REQUIRE("" == ml.at(1).get());
}

TEST_CASE("Buffer::RemoveRegion") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

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
    cu.at(0) = {0, 0};
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
    cu.at(0) = {0, 0};
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::Remove") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

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
    cu.at(0) = {1, 0};
    del = ml.remove();
    REQUIRE(1U == del.size());
    REQUIRE("#" == del[0]);
    REQUIRE(18 == ml.at(0).length());
    // undo
    ml.insert(del);
    REQUIRE(19 == ml.at(0).length());

    // remove at line beginning
    cu.at(0) = {0, 1};
    del = ml.remove();
    REQUIRE(1U == del.size());
    REQUIRE("\n" == del[0]);
    REQUIRE(20 == ml.length());
    REQUIRE(Pos2d<int>(19, 0) == cu.at(0));
    REQUIRE(66 == ml.at(0).length());
    // undo
    ml.insert(del);
    REQUIRE(Pos2d<int>(0, 1) == cu.at(0));
    REQUIRE(21 == ml.length());
    REQUIRE(19 == ml.at(0).length());
    REQUIRE(47 == ml.at(1).length());
}

TEST_CASE("Buffer::RemoveCurrent") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    // remove at the end of buffer
    cu.end(&ml);
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
    cu.reset(&ml);
    cu.right(&ml);
    del = ml.removeCurrent();
    REQUIRE(1U == del.size());
    REQUIRE("i" == del[0]);
    REQUIRE(18 == ml.at(0).length());
    // undo
    ml.insert(del);
    REQUIRE(19 == ml.at(0).length());

    // remove at line end
    cu.reset(&ml);
    cu.lineEnd(&ml);
    del = ml.removeCurrent();
    REQUIRE(1U == del.size());
    REQUIRE("\n" == del[0]);
    REQUIRE(20 == ml.length());
    REQUIRE(Pos2d<int>(19, 0) == cu.at(0));
    REQUIRE(66 == ml.at(0).length());
    // undo (it would certainly make the cursor move to next position!)
    ml.insert(del);
    REQUIRE(Pos2d<int>(0, 1) == cu.at(0));
    REQUIRE(21 == ml.length());
    REQUIRE(19 == ml.at(0).length());
    REQUIRE(47 == ml.at(1).length());
}

TEST_CASE("Buffer::GotoLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    ml.gotoLine(10);
    REQUIRE(Pos2d<int>(0, 10) == cu.at(0));
    ml.gotoLine(30);
    REQUIRE(Pos2d<int>(0, 20) == cu.at(0));
}

TEST_CASE("Buffer::SortRegionsEmptyLine") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    ml.enableRegions();
    auto pos = ml.getRegionLocs();
    cu.nextPara(&ml);
    auto del = ml.regionAsStr();
    REQUIRE(1U == del.size());
    REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif\n"
            == del[0]);
    REQUIRE(Pos2d<int>(0, 3) == cu.at(0));
    ml.sortRegions();
    auto after = cu.saveExcursion();
    ml.disableRegions();
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2d<int>(ml.at(3).length(), 3) == cu.at(0));
    REQUIRE("" == ml.at(0).get());
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
    REQUIRE("#endif" == ml.at(2).get());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(3).get());
    // undo
    ml.remove(pos, after);
    cu.restoreExcursion(pos);
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2d<int>(ml.at(3).length(), 3) == cu.at(0));
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
    REQUIRE("#endif" == ml.at(2).get());
    REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::SortRegions") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    ml.enableRegions();
    auto pos = ml.getRegionLocs();
    cu.nextPara(&ml);
    // don't include the empty line!
    cu.left(&ml);
    auto del = ml.regionAsStr();
    REQUIRE(1U == del.size());
    REQUIRE("#ifndef _GNU_SOURCE\n#define _GNU_SOURCE // for wcstring, strcasestr\n#endif"
            == del[0]);
    REQUIRE(Pos2d<int>(ml.at(2).length(), 2) == cu.at(0));
    ml.sortRegions();
    auto after = cu.saveExcursion();
    ml.disableRegions();
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2d<int>(ml.at(2).length(), 2) == cu.at(0));
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(0).get());
    REQUIRE("#endif" == ml.at(1).get());
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(2).get());
    // undo
    ml.remove(pos, after);
    cu.restoreExcursion(pos);
    ml.insert(del);
    REQUIRE(21 == ml.length());
    REQUIRE(Pos2d<int>(ml.at(2).length(), 2) == cu.at(0));
    REQUIRE("#ifndef _GNU_SOURCE" == ml.at(0).get());
    REQUIRE("#define _GNU_SOURCE // for wcstring, strcasestr" == ml.at(1).get());
    REQUIRE("#endif" == ml.at(2).get());
    REQUIRE("" == ml.at(3).get());
}

TEST_CASE("Buffer::KeepLinesNoMatches") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    REQUIRE(21 == ml.length());
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    Pcre pc("not there");
    auto res = ml.keepRemoveLines(pc, true);
    REQUIRE(21U == res.size());
    REQUIRE("#ifndef _GNU_SOURCE" == res[0].str);
    REQUIRE(Pos2d<int>(0, 0) == res[0].pos);
    REQUIRE("" == res[20].str);
    // pos is 0,0 because of sequential removal of lines!
    REQUIRE(Pos2d<int>(0, 0) == res[20].pos);
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
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    Pcre pc("include");
    auto res = ml.keepRemoveLines(pc, true);
    REQUIRE(9U == res.size());
    REQUIRE("#ifndef _GNU_SOURCE" == res[0].str);
    REQUIRE(Pos2d<int>(0, 0) == res[0].pos);
    REQUIRE("" == res[8].str);
    REQUIRE(Pos2d<int>(0, 12) == res[8].pos);
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
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
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
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    Pcre pc("include");
    auto res = ml.keepRemoveLines(pc, false);
    REQUIRE(12U == res.size());
    REQUIRE("#include <stdint.h>" == res[0].str);
    REQUIRE(Pos2d<int>(0, 4) == res[0].pos);
    REQUIRE("#include <algorithm>" == res[11].str);
    REQUIRE(Pos2d<int>(0, 4) == res[11].pos);
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
    auto& cu = ml.getCursor();
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
    cu.at(0) = {9, 4}; // '<'
    ml.matchCurrentParen();
    REQUIRE(Pos2d<int>(18, 4) == cu.at(0));
    ml.matchCurrentParen();
    REQUIRE(Pos2d<int>(9, 4) == cu.at(0));
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
    auto& cu = ml.getCursor();
    cu.lineEnd(&ml);
    ml.enableRegions();
    cu.down(&ml);
    cu.lineEnd(&ml);
    auto strs = ml.regionAsStr();
    REQUIRE(1U == strs.size());
    REQUIRE("\nTesting123" == strs[0]);
}

TEST_CASE("Buffer::Cut") {
    SECTION("Preceeding newline") {
        Buffer ml;
        setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
        REQUIRE(4 == ml.length());
        auto& cu = ml.getCursor();
        cu.lineEnd(&ml);
        ml.enableRegions();
        cu.down(&ml);
        cu.lineEnd(&ml);
        auto before = cu.saveExcursion();
        auto regs = ml.getRegionLocs();
        auto del = ml.remove(regs, before);
        REQUIRE(1U == del.size());
        REQUIRE("\nTesting123" == del[0]);
        REQUIRE(3 == ml.length());
    }
    SECTION("Succeeding newline") {
        Buffer ml;
        setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
        REQUIRE(4 == ml.length());
        auto& cu = ml.getCursor();
        cu.home(&ml);
        ml.enableRegions();
        cu.down(&ml);
        auto before = cu.saveExcursion();
        auto regs = ml.getRegionLocs();
        auto del = ml.remove(regs, before);
        REQUIRE(1U == del.size());
        REQUIRE("* Hello\n" == del[0]);
        REQUIRE(3 == ml.length());
    }
    SECTION("Preceeding & Succeeding newline") {
        Buffer ml;
        setupBuff(ml, {0, 0}, {30, 10}, "samples/multiline.txt", 0);
        REQUIRE(4 == ml.length());
        auto& cu = ml.getCursor();
        cu.lineEnd(&ml);
        ml.enableRegions();
        cu.down(&ml);
        cu.down(&ml);
        cu.home(&ml);
        auto before = cu.saveExcursion();
        auto regs = ml.getRegionLocs();
        auto del = ml.remove(regs, before);
        REQUIRE(1U == del.size());
        REQUIRE("\nTesting123\n" == del[0]);
        ///@todo: issue with trailing newline now!!
        REQUIRE(3 == ml.length());
    }
}

} // end namespace teditor
