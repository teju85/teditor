#include "editor.h"
#include "command.h"
#include <stdio.h>


namespace teditor {

std::string dirModeGetFileAtLine(const MultiLine& buf) {
    auto& culoc = buf.getCursor().at(0);
    if(culoc.y == 0)
        return "";
    auto& line = buf.at(culoc.y).get();
    return line.substr(buf.dirModeFileOffset());
}

CMD_NO_UNDO(DirModeOpenFile, "dirmode-open-file") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto file = dirModeGetFileAtLine(buf);
    auto& dir = buf.getFileName();
    if(file == "." || file.empty())
        return;
    if(file == "..") {
        ed.load(dirname(dir), 0);
        return;
    }
    ed.load(dir+'/'+file, 0);
}

CMD_NO_UNDO(DirModeCopyFile, "dirmode-copy-file") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto file = dirModeGetFileAtLine(buf);
    auto& dir = buf.getFileName();
    if(file == "." || file == ".." || file.empty())
        return;
    file = rel2abs(dir, file);
    ///@todo: support copying directories
    if(!isFile(file.c_str()))
        return;
    auto dst = ed.prompt("Dst file: ");
    dst = rel2abs(dir, dst);
    if(!dst.empty()) {
        MESSAGE("Copied '%s' to '%s'\n", file.c_str(), dst.c_str());
        copyFile(file, dst);
    }
}

CMD_NO_UNDO(DirModeRefresh, "dirmode-refresh") {
    auto& buf = Editor::getInstance().getBuff();
    buf.clear();
    buf.load(buf.getFileName(), 0);
}

CMD_NO_UNDO(DirModeRenameFile, "dirmode-rename-file") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto file = dirModeGetFileAtLine(buf);
    auto& dir = buf.getFileName();
    if(file == "." || file == ".." || file.empty())
        return;
    file = rel2abs(dir, file);
    auto dst = ed.prompt("Dst file: ");
    dst = rel2abs(dir, dst);
    if(!dst.empty()) {
        MESSAGE("Renamed '%s' to '%s'\n", file.c_str(), dst.c_str());
        rename(file.c_str(), dst.c_str());
    }
}

CMD_NO_UNDO(DirModeDeleteFile, "dirmode-delete-file") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto& dir = buf.getFileName();
    auto file = dirModeGetFileAtLine(buf);
    if(file == "." || file == ".." || file.empty())
        return;
    auto delFile = rel2abs(dir, file);
    ///@todo: support deleting directories
    if(!isFile(delFile.c_str()))
        return;
    if(ed.promptYesNo("Delete " + delFile + "?"))
        remove(delFile.c_str());
    buf.clear();
    buf.load(buf.getFileName(), 0);
}

} // end namespace teditor
