#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for wcstring, strcasestr
#endif

#include <stdint.h>
#include <vector>
#include "editor.h"
#include "keys.h"
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "utf8.h"
#include "command.h"
#include <algorithm>

namespace teditor {

void exitGracefully(int signum) {
    exit(signum);
}

const int Editor::OutBuffSize = 32 * 1024;

Editor* Editor::inst = nullptr;

void sigwinch_handler(int xxx) {
    (void) xxx;
    const int zzz = 1;
    write(Editor::getInstance().getWinchFd(1), &zzz, sizeof(int));
}

Editor::Editor(const Args& args_):
    outbuff(OutBuffSize), backbuff(), frontbuff(), tsize(), inout(-1), term(),
    winchFds(), tios(), origTios(), lastfg(), lastbg(), bufferResize(false),
    args(args_), cmBar(), buffs(), buffNames(), currBuff(0),
    quitEventLoop(false), quitPromptLoop(false), cancelPromptLoop(false),
    cmdMsgBarActive(false), copiedStr(), defcMap(),
    fileshist(args.getHistFile(), args.maxFileHistory) {
    inout = open(args.ttyFile.c_str(), O_RDWR);
    ASSERT(inout >= 0, "Failed to open tty '%s'!", args.ttyFile.c_str());
    ASSERT(pipe(winchFds) >= 0, "Failed to setup 'pipe'!");
    setSignalHandler();
    setupTios();
    outbuff.puts(term.func(Func_EnterKeypad));
    outbuff.puts(term.func(Func_EnterCA));
    populateColorMap<GlobalColors>(defcMap);
    resize();
    clearBackBuff();
    hideCursor();  // we'll manually handle the cursor draws
    input.reset();
}

Editor::~Editor() {
    showCursor();
    outbuff.puts(term.func(Func_Sgr0)); // reset attrs
    outbuff.puts(term.func(Func_ExitCA));
    outbuff.puts(term.func(Func_ExitKeypad));
    outbuff.puts(term.func(Func_ExitMouse));
    flush();
    for(auto itr : buffs)
        delete itr;
    fileshist.prune();
    fileshist.store();
    tcsetattr(inout, TCSAFLUSH, &origTios);
    close(winchFds[0]);
    close(winchFds[1]);
    close(inout);
}

void Editor::addFileHistory(const std::string& file, int line)  {
    fileshist.add(file, line);
}

std::vector<std::string> Editor::fileHistoryToString() const {
    return fileshist.toString();
}

std::vector<std::string> Editor::buffNamesToString() const {
    std::vector<std::string> ret;
    for(const auto* buff : buffs)
        ret.push_back(buff->bufferName());
    return ret;
}

int Editor::cmBarHeight() const {
    if(cmdMsgBarActive && cmBar.usingOptions())
        return args.cmdMsgBarMultiHeight;
    return args.cmdMsgBarHeight;
}

const AttrColor& Editor::getColor(const std::string& name) const {
    if(buffs.empty())
        return defcMap.get(name);
    return getBuff().getColor(name);
}

void Editor::runCmd(const std::string& cmd) {
    if(cmd.empty())
        return;
    auto c = Command::createCmd(cmd);
    bool undo = c->canUndo();
    auto& buf = getBuff();
    if(undo && buf.isRO()) {
        CMBAR_MSG("%s: Read Only Buffer!\n", cmd.c_str());
        return;
    }
    c->exec(CMD_FRESH);
    // check for undo status once more!
    if(c->canUndo())
        buf.addCommand(c);
}

void Editor::createScratchBuff(bool switchToIt) {
    auto bName = uniquifyName("*scratch", buffNames);
    auto* mlb = new MultiLine(bName);
    mlResize(mlb);
    buffs.push_back(mlb);
    buffNames.insert(mlb->bufferName());
    if(switchToIt)
        currBuff = (int)buffs.size() - 1;
}

///@todo: uniquify buffer names!
///@todo: don't load the same file/dir more than once!
void Editor::loadFiles() {
    DEBUG("loadFiles: started\n");
    currBuff = 0;
    if(args.files.empty()) {
        createScratchBuff();
        DEBUG("loadFiles: Added default buffer\n");
        return;
    }
    for(auto& f : args.files)
        load(f.first, f.second);
    DEBUG("loadFiles: ended\n");
}

void Editor::load(const std::string& file, int line) {
    auto* mlb = new MultiLine;
    mlResize(mlb);
    mlb->load(file, line);
    buffs.push_back(mlb);
    buffNames.insert(mlb->bufferName());
    currBuff = (int)buffs.size() - 1;
}

void Editor::incrementCurrBuff() {
    ++currBuff;
    if(currBuff >= (int)buffs.size())
        currBuff = 0;
}

void Editor::decrementCurrBuff() {
    --currBuff;
    if(currBuff < 0)
        currBuff = (int)buffs.size() - 1;
}

void Editor::switchToBuff(const std::string& name) {
    int idx = 0;
    for(const auto* buff : buffs) {
        if(name == buff->bufferName()) {
            currBuff = idx;
            break;
        }
        ++idx;
    }
}

void Editor::killCurrBuff() {
    checkForModifiedBuffer(buffs[currBuff]);
    buffNames.erase(buffs[currBuff]->bufferName());
    delete buffs[currBuff];
    buffs.erase(buffs.begin()+currBuff);
    if(buffs.empty()) {
        createScratchBuff();
        currBuff = 0;
        return;
    }
    if(currBuff >= (int)buffs.size())
        currBuff = 0;
}

void Editor::killOtherBuffs() {
    auto* mlb = buffs[currBuff];
    for(int i=0;i<(int)buffs.size();++i) {
        if(i != currBuff) {
            checkForModifiedBuffer(buffs[i]);
            delete buffs[i];
        }
    }
    buffs.clear();
    buffNames.clear();
    currBuff = 0;
    buffs.push_back(mlb);
    buffNames.insert(mlb->bufferName());
}

void Editor::run() {
    loadFiles();
    currBuff = 0;
    draw();
    render();
    quitEventLoop = false;
    TrieStatus state = TS_NULL;
    std::string keySoFar, currKey;
    while(true) {
        auto& kcMap = getBuff().getKeyCmdMap();
        int status = pollEvent();
        DEBUG("Editor:run: status=%d meta=%u key=%u keystr='%s'\n", status,
              input.mk.getMeta(), input.mk.getKey(), input.mk.toStr().c_str());
        if(status == Input::UndefinedSequence) {
            MESSAGE("Editor:run:: Undefined sequence: %s\n",
                    input.getOldSeq().c_str());
            continue;
        } else if(status < 0)
            break;
        switch(input.type) {
        case Event_Resize:
            clearBackBuff();
            resize();
            break;
        case Event_Key:
            currKey = input.mk.toStr();
            state = kcMap.traverse(currKey);
            if(state == TS_LEAF) {
                CMBAR("\n");
                std::string cmd = kcMap.getCmd();
                runCmd(cmd);
                kcMap.resetTraversal();
                keySoFar.clear();
            } else if(state == TS_NULL) {
                keySoFar += " " + currKey;
                CMBAR("Unknown key %s!\n", keySoFar.c_str());
                keySoFar.clear();
                kcMap.resetTraversal();
            } else {
                keySoFar += " " + currKey;
                CMBAR("%s - ", keySoFar.c_str());
            }
            break;
        ///@todo: mouse support
        default:
            break;
        }
        draw();
        render();
        if(quitEventLoop)
            break;
    }
}

void Editor::checkForModifiedBuffers() {
    for(auto itr : buffs)
        checkForModifiedBuffer(itr);
}

void Editor::checkForModifiedBuffer(MultiLine* mlb) {
    const auto& name = mlb->bufferName();
    if(!mlb->isModified() || mlb->isRO())
        return;
    // temp buffers need no saving
    if(name[0] == '*')
        return;
    if(promptYesNo("Buffer " + name + " modified. Save?"))
        mlb->save();
}

void Editor::sendf(const char* fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    std::string buf = format(fmt, vl);
    va_end(vl);
    return send(buf.c_str());
}

void Editor::updateTermSize() {
    struct winsize sz;
    memset(&sz, 0, sizeof(sz));
    ioctl(inout, TIOCGWINSZ, &sz);
    tsize.x = sz.ws_col;
    tsize.y = sz.ws_row;
}

void Editor::setSignalHandler() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigwinch_handler;
    sa.sa_flags = 0;
    sigaction(SIGWINCH, &sa, 0);
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = exitGracefully;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
    signal(SIGPIPE, SIG_IGN);
}

void Editor::setupTios() {
    tcgetattr(inout, &origTios);
    memcpy(&tios, &origTios, sizeof(tios));
    tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                      | INLCR | IGNCR | ICRNL | IXON); // INPCK
    tios.c_oflag &= ~OPOST;
    tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tios.c_cflag &= ~(CSIZE | PARENB);
    tios.c_cflag |= CS8;
    tios.c_cc[VMIN] = 0;  // Return each byte, or zero for timeout.
    tios.c_cc[VTIME] = 0; // 0ms timeout (unit is tens of second).
    tcsetattr(inout, TCSAFLUSH, &tios);
}

int Editor::sendString(int x, int y, const std::string& fg,
                       const std::string& bg, const char* str, int len) {
    Cell c = {0, getColor(fg), getColor(bg)};
    int count = 0;
    while(count < len) {
        count += Utf8::char2unicode(&c.ch, str+count);
        sendCell(x, y, c);
        ++x;
    }
    return count;
}

int Editor::sendChar(int x, int y, const std::string& fg, const std::string& bg,
                     char c) {
    Cell ce = {(Chr)c, getColor(fg), getColor(bg)};
    sendCell(x, y, ce);
    return 1;
}

int Editor::sendCharNtimes(int x, int y, const std::string& fg,
                           const std::string& bg, char c, int n) {
    Cell ce = {(Chr)c, getColor(fg), getColor(bg)};
    for(int i=0;i<n;++i)
        sendCell(x+i, y, ce);
    return n;
}

int Editor::sendStringf(int x, int y, const std::string& fg,
                        const std::string& bg, const char* fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    std::string buf = format(fmt, vl);
    va_end(vl);
    return sendString(x, y, fg, bg, buf.c_str(), (int)buf.size());
}

void Editor::resize() {
    updateTermSize();
    backbuff.resize(tsize.x, tsize.y);
    frontbuff.resize(tsize.x, tsize.y);
    const auto& defaultfg = getColor("defaultfg");
    const auto& defaultbg = getColor("defaultbg");
    frontbuff.clear(defaultfg, defaultbg);
    for(auto itr : buffs)
        mlResize(itr);
    clearScreen();
}

void Editor::clearBackBuff() {
    const auto& defaultfg = getColor("defaultfg");
    const auto& defaultbg = getColor("defaultbg");
    backbuff.clear(defaultfg, defaultbg);
}

MultiLine& Editor::getMessagesBuff() {
    MultiLine* mlb = getBuff("*messages");
    if(mlb != nullptr)
        return *mlb;
    auto* mlb1 = new MultiLine("*messages");
    mlResize(mlb1);
    buffs.push_back(mlb1);
    buffNames.insert(mlb1->bufferName());
    return *mlb1;
}

MultiLine* Editor::getBuff(const std::string& name) {
    for(auto itr : buffs)
        if(itr->bufferName() == name)
            return itr;
    return nullptr;
}

void Editor::writeLiteral(const char* fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    std::string buf = format(fmt, vl);
    va_end(vl);
    outbuff.append(buf.c_str(), (int)buf.size());
}

void Editor::writeCursor(int x, int y) {
    const auto& cursor = getBuff().getCursor();
    if(cursor.isHidden(x, y))
        return;
    writeLiteral("\033[%d;%dH", y+1, x+1);
}

void Editor::writeChar(uint32_t c, int x, int y) {
    char buf[8];
    int bw = Utf8::unicode2char(buf, c);
    if(!c)
        buf[0] = ' '; // replace 0 with whitespace
    writeCursor(x, y);
    outbuff.append(buf, bw);
}

void Editor::setColors(AttrColor fg, AttrColor bg) {
    if((fg == lastfg) && (bg == lastbg))
        return;
    lastfg = fg;
    lastbg = bg;
    outbuff.puts(term.func(Func_Sgr0)); // reset attrs
    if(fg.isBold())
        outbuff.puts(term.func(Func_Bold));
    if(fg.isUnderline())
        outbuff.puts(term.func(Func_Underline));
    uint8_t fgcol = (uint8_t)fg.color();
    uint8_t bgcol = (uint8_t)bg.color();
    writeLiteral("\033[38;5;%d;48;5;%dm", fgcol, bgcol);
}

void Editor::clearScreen() {
    const auto& defaultfg = getColor("defaultfg");
    const auto& defaultbg = getColor("defaultbg");
    setColors(defaultfg, defaultbg);
    outbuff.puts(term.func(Func_ClearScreen));
    clearBackBuff();
    render();
}

void Editor::Prompter::init() {
}

void Editor::Prompter::loop() {
}

void Editor::Prompter::deinit() {
}

bool Editor::promptYesNo(const std::string& msg) {
    auto msg_ = msg + " (y/n)? ";
    KeyCmdMap ynMap;
    populateKeyMap<PromptYesNoKeys>(ynMap, true);
    auto res = prompt(msg_, &ynMap);
    return (res == "y");
}

std::string Editor::promptEnum(const std::string& msg, OptionMap& opts) {
    std::string m(msg);
    std::unordered_map<char, std::string> letters;
    KeyCmdMap eMap;
    for(const auto& itr : opts) {
        m += ' ' + itr.first;
        letters[itr.first[0]] = itr.first;
        eMap.add(itr.first[0], "prompt-insert-char-quit");
    }
    m += ": ";
    eMap.add("esc", "prompt-cancel");
    const auto res = prompt(m, &eMap);
    // cancel requested
    if(res.empty())
        return "";
    const auto itr = letters.find(res[0]);
    return opts[itr->second];
}

std::string Editor::prompt(const std::string& msg, KeyCmdMap* kcMap,
                           const std::vector<std::string>* opts) {
    selectCmBar();
    if(kcMap == nullptr)
        kcMap = &(cmBar.getKeyCmdMap());
    if(opts != nullptr)
        cmBar.setOptions(*opts);
    int msgLen = (int)msg.size();
    cmBar.setMinLoc(msgLen);
    mlResize(&getBuff());
    quitPromptLoop = cancelPromptLoop = false;
    cmBar.insert(msg.c_str());
    std::string currKey;
    TrieStatus state = TS_NULL;
    draw();
    render();
    while(!quitPromptLoop) {
        int status = pollEvent();
        DEBUG("Editor:prompt: status=%d meta=%u key=%u keystr='%s'\n", status,
              input.mk.getMeta(), input.mk.getKey(), input.mk.toStr().c_str());
        if(status == Input::UndefinedSequence) {
            MESSAGE("Editor:prompt:: Undefined sequence: %s\n",
                    input.getOldSeq().c_str());
            break;
        } else if(status < 0)
            break;
        if(input.type == Event_Key) {
            currKey = input.mk.toStr();
            state = kcMap->traverse(currKey);
            if(state == TS_LEAF) {
                std::string cmd = kcMap->getCmd();
                runCmd(cmd);
                kcMap->resetTraversal();
            } else if(state == TS_NULL)
                kcMap->resetTraversal();
        }
        draw();
        render();
    }
    auto ret = cmBar.usingOptions()? cmBar.getOptStr() : cmBar.getStr();
    if(cancelPromptLoop)
        ret.clear();
    cmBar.clear();
    cmBar.setMinLoc(0);
    if(opts != nullptr)
        cmBar.clearOptions();
    unselectCmBar();
    mlResize(&getBuff());
    return ret;
}

void Editor::draw() {
    auto& buff = getBuff();
    clearBackBuff();
    DEBUG("draw: drawBuffer\n");
    buff.drawBuffer(*this);
    DEBUG("draw: drawStatusBar\n");
    buff.drawStatusBar(*this);
    DEBUG("draw: cmdMsgBar.drawBuffer\n");
    cmBar.drawBuffer(*this);
    if(cmdMsgBarActive) {
        DEBUG("draw: cmdMsgBar.drawCursor\n");
        cmBar.drawCursor(*this, "cursorbg");
        buff.drawCursor(*this, "inactivecursorbg");
    } else {
        DEBUG("draw: drawCursor\n");
        buff.drawCursor(*this, "cursorbg");
    }
    DEBUG("draw: ended\n");
}

void Editor::mlResize(MultiLine* mlb) {
    int ht = cmBarHeight();
    auto sz = tsize;
    sz.y -= ht;
    mlb->resize({0, 0}, sz);
    cmBar.resize({0, sz.y}, {tsize.x, ht});
    DEBUG("mlResize: buff-x,y=%d,%d ht=%d\n", sz.x, sz.y, ht);
}

void Editor::render() {
    if(bufferResize) {
        bufferResize = false;
        resize();
    }
    int h = (int)frontbuff.h();
    int w = (int)frontbuff.w();
    for(int y=0;y<h;++y) {
        for(int x=0;x<w;) {
            const Cell& back = backbuff.at(x, y);
            Cell& front = frontbuff.at(x, y);
            int wid = back.width();
            if(wid < 1)
                wid = 1;
            ULTRA_DEBUG("render: y,x=%d,%d back=%u,%hu,%hu front=%u,%hu,%hu\n",
                        y, x, back.ch, back.fg, back.bg, front.ch, front.fg,
                        front.bg);
            if(front == back) {
                x += wid;
                continue;
            }
            front.copy(back);
            setColors(back.fg, back.bg);
            // if character exceeds the screen width
            if(x >= w - wid + 1) {
                ASSERT(false, "Character exceeding screen width is not supported!");
                for (int i=x;i<w;++i)
                    writeChar(' ', i, y);
            } else {
                writeChar(back.ch, x, y);
            }
            x += wid;
        }
    }
    flush();
}

int Editor::pollEvent() {
    return input.waitAndFill(nullptr, *this);
}

int Editor::peekEvent(int timeoutMs) {
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs - (tv.tv_sec * 1000)) * 1000;
    return input.waitAndFill(&tv, *this);
}

} // end namespace teditor
