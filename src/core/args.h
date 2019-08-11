#pragma once

#include <vector>
#include <string>
#include "colors.h"
#include <utility>
#include "file_utils.h"


namespace teditor {

struct Args {
  Args(int argc, char** argv);
  std::string wrtHomeFolder(const std::string& s) const;
  std::string getHistFile() const { return getFile(histFile); }
  std::string getLedgerFile() const { return getFile(ledgerFile); }

  bool quitAfterLoad;
  std::string ttyFile, homeFolder, ledgerFile, orgNotesDir;
  std::vector<FileInfo> files;
  int logLevel, cmdMsgBarHeight, cmdMsgBarMultiHeight;
  float pageScrollJump;
  int maxFileHistory, tabSpaces;
  std::string histFile, browserCmd;
  char winSplitChar;

private:
  std::string getFile(const std::string& f) const;
};

} // end namespace teditor
