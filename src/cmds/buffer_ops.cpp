#include "core/editor.h"
#include "core/command.h"


namespace teditor {
namespace BufferOps {

DEF_CMD(Save, "save-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRO()) {
                CMBAR_MSG("save-buffer: Read Only Buffer!\n");
                return;
            }
            buf.save();
        },
        DEF_HELP() {
            return "Saves contents of the current buffer to the filesystem.";
        });

DEF_CMD(Pwd, "pwd",
        DEF_OP() {
            const auto& buf = ed.getBuff();
            CMBAR_MSG("Pwd: %s\n", buf.pwd().c_str());
        },
        DEF_HELP() {
            return "Prints the current working directory of the buffer.";
        });

DEF_CMD(Next, "next-buffer",
        DEF_OP() { ed.incrementCurrBuff(); },
        DEF_HELP() {
            return "Switch to the next buffer";
        });

DEF_CMD(Previous, "prev-buffer",
        DEF_OP() { ed.decrementCurrBuff(); },
        DEF_HELP() {
            return "Switch to the previous buffer";
        });

DEF_CMD(KillThis, "kill-this-buffer",
        DEF_OP() { ed.killCurrBuff(); },
        DEF_HELP() {
            return "Kill the current buffer. It'll check for this buffer to"
                " be modified but unsaved and if so, will prompt for saving.";
        });

DEF_CMD(KillOthers, "kill-other-buffers",
        DEF_OP() { ed.killOtherBuffs(); },
        DEF_HELP() {
            return "Kill other buffers. It'll check for any modified but"
                " unsaved buffers and will prompt them to be saved.";
        });

DEF_CMD(Reload, "reload-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isModified() &&
               ed.promptYesNo("Buffer modified, still reload? "))
                buf.reload();
        },
        DEF_HELP() {
            return "Reloads the buffer contents from the filesystem. Note that"
                " this will overwrite any modified-but-unsaved changes in the"
                " current buffer!";
        });

} // end namespace BufferOps
} // end namespace teditor
