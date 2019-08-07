#include "core/utils.h"
#include <cstdio>
#include "core/args.h"
#include "core/editor.h"
#include "core/terminal.h"
#include "core/logger.h"


int main(int argc, char** argv) {
  using namespace teditor;
  ///@todo: add more relevant catch statements in future!
  try {
    SingletonHandler<Logger, std::string> shl("debug.log");
    Args args(argc, argv);
    SingletonHandler<Terminal, std::string> term(args.ttyFile);
    auto* ed = new Editor(args);
    ed->run();
    DEBUG("Exited Editor::run loop...\n");
    delete ed;
    DEBUG("Editor: dtor finished\n");
    DEBUG("Closing teditor\n");
    return 0;
  } catch(const std::runtime_error& e) {
    printf("teditor: Error: %s\n", e.what());
    return -1;
  } catch(...) {
    printf("teditor: Unknown exception occured!\n");
    return -2;
  }
}
