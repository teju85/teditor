#include "core/utils.h"
#include <cstdio>
#include "core/editor.h"
#include "core/terminal.h"
#include "core/logger.h"
#include "core/option.h"
#include "core/file_utils.h"
#include <memory>

int main(int argc, char** argv) {
  using namespace teditor;
  ///@todo: add more relevant catch statements in future!
  try {
    SingletonHandler<Logger, std::string> shl("debug.log");
    std::vector<FileInfo> files;
    if (!parseArgs(argc, argv, files)) return 0;
    Logger::setLevel(Option::get("logLevel").getInt());
    makeDir(Option::get("homeFolder").getStr());
    SingletonHandler<Terminal, std::string> term(Option::get("tty").getStr());
    {
      std::shared_ptr<Editor> ed(new Editor(files));
      ed->setTitle(Option::get("title").getStr());
      ed->run();
    }
    DEBUG("Editor: dtor finished. Closing teditor\n");
    return 0;
  } catch(const std::runtime_error& e) {
    printf("teditor: Error: %s\n", e.what());
    return -1;
  } catch(...) {
    printf("teditor: Unknown exception occured!\n");
    return -2;
  }
}
