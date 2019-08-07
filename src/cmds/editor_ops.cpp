#include "core/editor.h"
#include "core/command.h"


namespace teditor {
namespace EditorOps {

DEF_CMD(Quit, "quit",
        DEF_OP() {
            ed.requestQuitEventLoop();
            ed.checkForModifiedBuffers();
        },
        DEF_HELP() {
            return "Quit the editor. It'll check for any modified but unsaved"
                " buffers and prompt for saving them.";
        });

DEF_CMD(HelpCommand, "help-command",
        DEF_OP() {
            StringChoices sc(allCmdNames());
            auto cmd = ed.prompt("Help for Cmd? ", nullptr, &sc);
            if(cmd.empty()) return;
            try {
                auto contents = getCmd(cmd).second(ed);
                std::string name = "*help-" + cmd;
                ed.createReadOnlyBuff(name, contents, true);
            } catch(const std::runtime_error& e) {
                CMBAR_MSG(ed, "Unknown command: %s!\n", cmd.c_str());
            }
        },
        DEF_HELP() {
            return "Prints help messages for a given command. It creates a new"
                " read-only buffer for doing so.";
        });

DEF_CMD(ScratchBuffer, "scratch-buffer",
        DEF_OP() { ed.createScratchBuff(true); },
        DEF_HELP() { return "Create a scratch buffer and switch to it"; });

DEF_CMD(FindFileHistory, "find-file-history",
        DEF_OP() {
            StringChoices sc(ed.fileHistoryToString());
            auto file = ed.prompt("Find File History: ", nullptr, &sc);
            if(file.empty()) return;
            auto fi = readFileInfo(file);
            ed.load(fi.first, fi.second);
        },
        DEF_HELP() {
            return "Opens a prompt with a list of previously opened files for a"
                " speedier finding of a file. History file is always stored at"
                " $homeFolder/history. The arg 'homeFolder' can be customized"
                " as it is the folder which has all teditor related settings."
                " The arg maxFileHistory controls the amount of previous files"
                " to be remembered.";
        });

DEF_CMD(
  VerticalSplit, "split-vertically", DEF_OP() {
    auto status = ed.splitVertically();
    if(!status) MESSAGE(ed, "Window already split vertically");
  },
  DEF_HELP() { return "Splits the current window vertically (only once!)."; });

DEF_CMD(ClearAllWindows, "clear-all-windows",
        DEF_OP() { ed.clearAllWindows(); },
        DEF_HELP() { return "Merge all split windows into one."; });

DEF_CMD(NextWindow, "next-window",
        DEF_OP() { ed.incrementCurrWin(); },
        DEF_HELP() { return "Shift focus on the next window."; });

class FileChoices: public StringChoices {
public:
    FileChoices(const Strings& arr, ChoicesFilter cf=fileStrFind):
        StringChoices(arr, cf) {
    }

    bool updateChoices(const std::string& str) {
        if(str.empty() || str.back() != '/')
            return false;
        options = listDirRel(str);
        return true;
    }

    std::string getFinalStr(int idx, const std::string& str) const {
        auto loc = str.find_last_of('/');
        if(loc == std::string::npos)
            return str;
        auto sub = str.substr(0, loc+1) + at(idx);
        return sub;
    }
};

DEF_CMD(FindFile, "find-file",
        DEF_OP() {
            auto& buf = ed.getBuff();
            auto pwd = buf.pwd() + '/';
            FileChoices sc(listDirRel(pwd));
            auto file = ed.prompt("Find File: ", nullptr, &sc, pwd);
            if(!file.empty()) ed.load(file, 0);
        },
        DEF_HELP() { return "Opens a prompt for user to find a file."; });

DEF_CMD(RunCommand, "run-command",
        DEF_OP() {
            StringChoices sc(allCmdNames());
            auto cmd = ed.prompt("Cmd: ", nullptr, &sc);
            if(cmd.empty()) return;
            try {
                ed.runCmd(cmd);
            } catch(const std::runtime_error& e) {
                CMBAR_MSG(ed, "Unknown command: %s!\n", cmd.c_str());
            }
        },
        DEF_HELP() {
            return "Prompts user to choose a command from the current database"
                " and runs it.";
        });

DEF_CMD(ShellCommand, "shell-command",
        DEF_OP() {
            auto cmd = ed.prompt("Shell Command: ");
            if(!cmd.empty()) {
                auto res = check_output(cmd);
                MESSAGE(ed, "Shell Command: %s (exit-status=%d)\nOutput: %s",
                        cmd.c_str(), res.status, res.output.c_str());
            }
        },
        DEF_HELP() {
            return "Prompts user to type in a shell command to run. The current"
                " working dir for running this command will be the same as that"
                " of the current buffer.";
        });

DEF_CMD(LaunchExplorer, "open-explorer",
        DEF_OP() { check_output("cygstart " + ed.getBuff().pwd()); },
        DEF_HELP() {
            return "Opens the file explorer. Currently only works on cygwin!";
        });

DEF_CMD(LaunchBrowser, "browser",
        DEF_OP() {
            auto& buf = ed.getBuff();
            auto& args = ed.getArgs();
            std::string url;
            if(buf.isRegionActive()) url = buf.regionAsStr()[0];
            check_output(args.browserCmd + " '" + url + "'");
        },
        DEF_HELP() {
            return "Opens the url pointed by the current active region in the"
                " browser. If no active regions, it'll just open the browser."
                " The arg 'browserCmd' can be used to customize the command"
                " used to launch the browser.";
        });

DEF_CMD(BrowserSearch, "browser-search",
        DEF_OP() {
            OptionMap opts;
            opts["duckduckgo"] = "http://www.duckduckgo.com/?t=hb&ia=meanings&q=%s";
            opts["google"] = "http://www.google.com/#q=%s";
            opts["maps"] = "https://www.google.co.in/maps/search/%s";
            opts["nvsearch"] = "https://nvsearch.nvidia.com/Pages/results.aspx?k=%s";
            opts["stock"] = "https://duckduckgo.com/?q=%s&t=ffab&ia=stock";
            opts["youtube"] = "https://www.youtube.com/results?search_query=%s";
            auto& args = ed.getArgs();
            auto command = ed.promptEnum("Search:", opts);
            if(command.empty()) return;
            command = args.browserCmd + " '" + command + "'";
            // we'll only look at first cursor!
            auto query = ed.getBuff().regionAsStr();
            auto queryStr = query.empty()? ed.prompt("Query: ") : query[0];
            if(queryStr.empty()) return;
            auto hexified = urlHexify(queryStr);
            auto buf = format(command.c_str(), hexified.c_str());
            check_output(buf.c_str());
        },
        DEF_HELP() {
            return "Opens a prompt to search for a given string among various"
                " search options. The given string is first tried for in the"
                " currently active region, if not, it'll be prompted for.";
        });

DEF_CMD(Download, "download-url",
        DEF_OP() {
            auto url = ed.prompt("URL: ");
            if(url.empty()) return;
            auto file = ed.prompt("File: ");
            if(file.empty()) return;
            downloadUrl(url, file);
        },
        DEF_HELP() {
            return "Prompts for a URL and a filename and dowloads that URL into"
                " that file.";
        });

DEF_CMD(MakeDir, "mkdir",
        DEF_OP() {
            auto res = ed.prompt("Dir name: ");
            if(!res.empty()) makeDir(res);
        },
        DEF_HELP() { return "Creates a directory by prompting the user."; });

DEF_CMD(SwitchBuffer, "buffer-switch",
        DEF_OP() {
            StringChoices sc(ed.buffNamesToString());
            auto buf = ed.prompt("Buffer: ", nullptr, &sc);
            if(!buf.empty()) ed.switchToBuff(buf);
        },
        DEF_HELP() { return "Switches to a user selected buffer."; });

DEF_CMD(GitBranch, "git-branch",
        DEF_OP() {
            auto& buf = ed.getBuff();
            auto br = gitBranchName(buf.pwd());
            if(br.empty())
                CMBAR(ed, "Not a git-repo!\n");
            else
                CMBAR(ed, "git-branch=%s\n", br.c_str());
        },
        DEF_HELP() { return "Prints the git branch of the current dir."; });

DEF_CMD(IndentLine, "indent",
        DEF_OP() { ed.getBuff().indent(); },
        DEF_HELP() { return "Indents the current line."; });

DEF_CMD(OrgNotesDir, "org-notes-dir",
        DEF_OP() {
            const auto& args = ed.getArgs();
            ed.load(args.orgNotesDir, 0);
        },
        DEF_HELP() {
            return "Loads the dir containing all org files. The dir can be"
                " parameterized by the arg 'orgNotesDir'.";
        });

DEF_CMD(TaskManager, "task-manager",
        DEF_OP() {
            std::string cmd = "cygstart Taskmgr.exe";
            auto res = check_output(cmd);
            MESSAGE(ed, "Shell Command: %s (exit-status=%d)\nOutput: %s",
                    cmd.c_str(), res.status, res.output.c_str());
        },
        DEF_HELP() { return "Starts task manager UI. Only for windows!"; });

} // end namespace EditorOps
} // end namespace teditor
