#include "args.h"
#include <cstring>
#include "logger.h"
#include <stdlib.h>


namespace teditor {

std::string Args::wrtHomeFolder(const std::string& s) const {
  return homeFolder + '/' + s;
}

std::string Args::getFile(const std::string& f) const {
  if(isAbs(f)) return f;
  return wrtHomeFolder(f);
}

Args::Args(int argc, char** argv):
  quitAfterLoad(false), ttyFile("/dev/tty"), homeFolder("$HOME/.teditor"),
  ledgerFile("ledger.lg"), orgNotesDir("/cygdrive/c/Devtech/notes"), files(),
  logLevel(-1), cmdMsgBarHeight(1), cmdMsgBarMultiHeight(8),
  pageScrollJump(0.75f), maxFileHistory(20), tabSpaces(4), histFile("history"),
  browserCmd("cygstart firefox -private-window"), winSplitChar('|') {
  for(int i=1;i<argc;++i) {
    if(!strcmp(argv[i], "-Q")) {
      quitAfterLoad = true;
    } else if(!strcmp(argv[i], "-v")) {
      ++i;
      ASSERT(i < argc, "'-v' option expects an argument!");
      logLevel = str2num(argv[i]);
    } else if(!strcmp(argv[i], "-tty")) {
      ++i;
      ASSERT(i < argc, "'-tty' option expects an argument!");
      ttyFile = argv[i];
    } else if(!strcmp(argv[i], "-home")) {
      ++i;
      ASSERT(i < argc, "'-home' option expects an argument!");
      homeFolder = argv[i];
    } else if(!strcmp(argv[i], "-ledger-file")) {
      ++i;
      ASSERT(i < argc, "'-ledger-file' expects an argument!");
      ledgerFile = argv[i];
    } else if(!strcmp(argv[i], "-org-notes-dir")) {
      ++i;
      ASSERT(i < argc, "'-org-notes-dir' option expects an argument!");
      orgNotesDir = argv[i];
    } else if(!strcmp(argv[i], "-cmd-msg-bar-height")) {
      ++i;
      ASSERT(i < argc, "'-cmd-msg-bar-height' expects an argument!");
      cmdMsgBarHeight = str2num(argv[i]);
    } else if(!strcmp(argv[i], "-cmd-msg-bar-multi-height")) {
      ++i;
      ASSERT(i < argc, "'-cmd-msg-bar-multi-height' expects an argument!");
      cmdMsgBarMultiHeight = str2num(argv[i]);
    } else if(!strcmp(argv[i], "-page-scroll-jump")) {
      ++i;
      ASSERT(i < argc, "'-page-scroll-jump' option expects an argument!");
      pageScrollJump = str2real(argv[i]);
    } else if(!strcmp(argv[i], "-max-file-history")) {
      ++i;
      ASSERT(i < argc, "'-max-file-history' option expects an argument!");
      maxFileHistory = str2num(argv[i]);
    } else if(!strcmp(argv[i], "-browser-cmd")) {
      ++i;
      ASSERT(i < argc, "'-browser-cmd' option expects an argument!");
      browserCmd = argv[i];
    } else if(!strcmp(argv[i], "-tab-spaces")) {
      ++i;
      ASSERT(i < argc, "'-tab-spaces' option expects an argument!");
      tabSpaces = str2num(argv[i]);
    } else if(!strcmp(argv[i], "-win-split-char")) {
      ++i;
      ASSERT(i < argc, "'-win-split-char' option expects an argument!");
      winSplitChar = argv[i][0];
    } else {
      ASSERT(argv[i][0] != '-', "Invalid arg passed! '%s'", argv[i]);
      files.push_back(readFileInfo(argv[i]));
    }
  }
  Logger::setLevel(logLevel);
  homeFolder = expandEnvVars(homeFolder);
  makeDir(homeFolder);
}

}; // end namespace teditor
