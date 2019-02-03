#include "core/utils.h"
#include <cstdio>
#include "core/args.h"
#include "core/editor.h"
#include "core/terminal.h"


int main(int argc, char** argv) {
    using namespace teditor;
    ///@todo: add more relevant catch statements in future!
    try {
        SingletonHandler<Logger, std::string> shl("debug.log");
        Args args(argc, argv);
        SingletonHandler<Terminal, std::string> term(args.ttyFile);
        Editor ed(args, Terminal::getInstance());
        ed.run();
    } catch(const std::runtime_error& e) {
        printf("teditor: Error: %s\n", e.what());
        return -1;
    } catch(...) {
        printf("teditor: Unknown exception occured!\n");
        return -2;
    }
    return 0;
}
