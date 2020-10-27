#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"
#include "core/net_utils.h"

namespace teditor {
namespace editor {
namespace ops {

/**
 * @page editor_ops Operations in a session
 * All operations that can be performed inside the Editor session.
 *
 * @tableofcontents
 */

/**
 * @page editor_ops
 * @section quit
 * Quit the editor session. It'll check for any modified-but-unsaved buffers and
 * prompts for saving them, before exiting the session.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(Quit, "quit", "editor_ops",
        DEF_OP() { ed.requestQuitEventLoop(); });

/**
 * @page editor_ops
 * @section help-command
 * Prints helps messages for a given command. It creates a new read-only Buffer
 * for doing so.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(HelpCommand, "help-command", "editor_ops", DEF_OP() {
    StringChoices sc(ed.getBuff().cmdNames());
    auto cmd = ed.prompt("Help for Cmd? ", nullptr, &sc);
    if(cmd.empty()) return;
    try {
      // auto contents = getCmd(cmd).second(ed);
      // std::string name = "*help-" + cmd;
      // ed.createReadOnlyBuff(name, contents, true);
      CMBAR_MSG(ed, "TODO\n");
    } catch(const std::runtime_error& e) {
      CMBAR_MSG(ed, "Unknown command: %s!\n", cmd.c_str());
    }
  });

/**
 * @page editor_ops
 * @section scratch-buffer
 * Start a `*scratch` Buffer if it is not present and switches to it.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(ScratchBuffer, "scratch-buffer", "editor_ops",
        DEF_OP() { ed.createScratchBuff(true); });

/**
 * @page editor_ops
 * @section find-file-histoy
 * Opens a prompt with a list of previously opened files for a speedier finding
 * of a file. History file is always stored at `<homeFolder>/history`.
 *
 * The Option `homeFolder` can be customized as it is the folder which has all
 * the teditor related settings. The Option `maxFileHistory` controls the number
 * of previous files to be remembered across sessions.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(FindFileHistory, "find-file-history", "editor_ops", DEF_OP() {
    StringChoices sc(ed.fileHistoryToString());
    auto file = ed.prompt("Find File History: ", nullptr, &sc);
    if(file.empty()) return;
    auto fi = readFileInfo(file);
    ed.load(fi.first, fi.second);
  });

/**
 * @page editor_ops
 * @section split-vertically
 * Splits the current window vertically.
 *
 * @note Available since v1.0.0
 * @note Currently only one split is supported.
 */
DEF_CMD(VerticalSplit, "split-vertically", "editor_ops", DEF_OP() {
    auto status = ed.splitVertically();
    if(!status) MESSAGE(ed, "Window already split vertically");
  });

/**
 * @page editor_ops
 * @section clear-all-windows
 * Merge all split windows into one.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(ClearAllWindows, "clear-all-windows", "editor_ops",
        DEF_OP() { ed.clearAllWindows(); });

/**
 * @page editor_ops
 * @section next-window
 * Shift the focus on the next Window.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(NextWindow, "next-window", "editor_ops",
        DEF_OP() { ed.incrementCurrWin(); });

class FileChoices: public StringChoices {
public:
  FileChoices(const Strings& arr, ChoicesFilter cf=fileStrFind):
    StringChoices(arr, cf) {
  }

  bool updateChoices(const std::string& str) {
    if(str.empty() || str.back() != '/') return false;
    options = DirCache::getDirContents(str);
    return true;
  }

  std::string getFinalStr(int idx, const std::string& str) const {
    auto loc = str.find_last_of('/');
    if(loc == std::string::npos) return str;
    auto sub = str.substr(0, loc+1) + at(idx);
    return sub;
  }
};

/**
 * @page editor_ops
 * @section find-file
 * Opens a prompt for user to find a file and opens it in a new Buffer. If that
 * file is already open in the current session, it'll just switch over to that
 * Buffer instead.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(FindFile, "find-file", "editor_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    auto pwd = buf.pwd() + '/';
    FileChoices sc(DirCache::getDirContents(pwd));
    auto file = ed.prompt("Find File: ", nullptr, &sc, pwd);
    if(!file.empty()) ed.load(file, 0);
  });

/**
 * @page editor_ops
 * @section run-command
 * Prompts the user to choose a command from the current list of supported
 * commands and runs it.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(RunCommand, "run-command", "editor_ops", DEF_OP() {
    StringChoices sc(ed.getBuff().cmdNames());
    auto cmd = ed.prompt("Cmd: ", nullptr, &sc);
    if(cmd.empty()) return;
    try {
      ed.runCmd(cmd);
    } catch(const std::runtime_error& e) {
      CMBAR_MSG(ed, "Unknown command: %s! Reason: %s\n", cmd.c_str(), e.what());
    }
  });

/**
 * @page editor_ops
 * @section shell-command
 * Prompts the user to type in a shell command to run. The current working dir
 * for this command will be the same as that of the current Buffer. In case if
 * the current dir is remote, the command will be executed instead on that
 * remote machine.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(ShellCommand, "shell-command", "editor_ops", DEF_OP() {
    auto cmd = ed.prompt("Shell Command: ");
    if(!cmd.empty()) {
      auto res = check_output(cmd);
      MESSAGE(ed, "Shell Command: %s (exit-status=%d)\nOutput: %s\nError: %s\n",
              cmd.c_str(), res.status, res.output.c_str(), res.error.c_str());
    }
  });

/**
 * @page editor_ops
 * @section open-explorer
 * Opens the file explorer.
 *
 * @note Available since v1.0.0
 * @note Currently available only on Windows OS.
 */
DEF_CMD(LaunchExplorer, "open-explorer", "editor_ops",
        DEF_OP() { check_output("cygstart " + ed.getBuff().pwd()); });

/**
 * @page editor_ops
 * @section browser
 * Opens the url pointed by the current active region in the browser. If no
 * active region, it'll just open the browser. The Option `browserCmd` can be
 * used to customize the command used for launching the browser.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(LaunchBrowser, "browser", "editor_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    std::string url;
    if(buf.isRegionActive()) url = buf.regionAsStr()[0];
    check_output(Option::get("browserCmd").getStr() + " '" + url + "'");
  });

/**
 * @page editor_ops
 * @section browser-search
 * Opens a prompt to search for a given string among various search options.
 * The given string is first tried for in the currently active region, if not,
 * it'll be prompted for.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(BrowserSearch, "browser-search", "editor_ops", DEF_OP() {
    OptionMap opts;
    opts["duckduckgo"] = "http://www.duckduckgo.com/?t=hb&ia=meanings&q=%s";
    opts["google"] = "http://www.google.com/#q=%s";
    opts["maps"] = "https://www.google.co.in/maps/search/%s";
    opts["nvsearch"] = "https://nvsearch.nvidia.com/Pages/results.aspx?k=%s";
    opts["stock"] = "https://duckduckgo.com/?q=%s&t=ffab&ia=stock";
    opts["youtube"] = "https://www.youtube.com/results?search_query=%s";
    auto command = ed.promptEnum("Search:", opts);
    if(command.empty()) return;
    command = Option::get("browserCmd").getStr() + " '" + command + "'";
    auto query = ed.getBuff().regionAsStr();
    if (query.empty()) query = ed.prompt("Query: ");
    if(query.empty()) return;
    auto hexified = urlHexify(query);
    auto buf = format(command.c_str(), hexified.c_str());
    check_output(buf.c_str());
  });

/**
 * @page editor_ops
 * @section download-url
 * Prompts for a URL and a filename and downloads that URL into this file.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(Download, "download-url", "editor_ops", DEF_OP() {
    auto url = ed.prompt("URL: ");
    if(url.empty()) return;
    auto file = ed.prompt("File: ");
    if(file.empty()) return;
    downloadUrl(url, file, Option::get("dnldProg").getStr(),
                Option::get("dnldProgOpts").getStr());
  });

/**
 * @page editor_ops
 * @section mkdir
 * Creates a directory by prompting for its path.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(MakeDir, "mkdir", "editor_ops", DEF_OP() {
    auto res = ed.prompt("Dir name: ");
    if(!res.empty()) makeDir(res);
  });

/**
 * @page editor_ops
 * @section buffer-switch
 * Switches to the user selected Buffer from the list of current buffers in the
 * session.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(SwitchBuffer, "buffer-switch", "editor_ops", DEF_OP() {
    StringChoices sc(ed.buffNamesToString());
    auto buf = ed.prompt("Buffer: ", nullptr, &sc);
    if(!buf.empty()) ed.switchToBuff(buf);
  });

/**
 * @page editor_ops
 * @section git-branch
 * Prints the git branch of the current directory into the command status bar.
 * It expects the `git` executable to be made available in the `PATH`
 * environment variable of the shell.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(GitBranch, "git-branch", "editor_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    auto br = gitBranchName(buf.pwd());
    if(br.empty())
      CMBAR(ed, "Not a git-repo!\n");
    else
      CMBAR(ed, "git-branch=%s\n", br.c_str());
  });

/**
 * @page editor_ops
 * @section indent
 * Indents the current line. Definition of indent and de-indent is as per the
 * Mode of the current Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(IndentLine, "indent", "editor_ops",
        DEF_OP() { ed.getBuff().indent(); });

/**
 * @page editor_ops
 * @section org-notes-dir
 * Loads the dir containing all the org notes files. Theis dir can be customized
 * using the Option `orgNotesDir`.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(OrgNotesDir, "org-notes-dir", "editor_ops", DEF_OP() {
    ed.load(Option::get("orgNotesDir").getStr(), 0);
  });

/**
 * @page editor_ops
 * @section songs-dir
 * Loads the dir containing songs. This dir location can be customized using the
 * Option `songsDir`.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(SongsDir, "songs-dir", "editor_ops",
        DEF_OP() { ed.load(Option::get("songsDir").getStr(), 0); });

/**
 * @page editor_ops
 * @section task-manager
 * Starts the task manager UI.
 *
 * @note Available since v1.0.0
 * @note Currently only supports Windows OS
 */
DEF_CMD(TaskManager, "task-manager", "editor_ops", DEF_OP() {
    std::string cmd = "cygstart Taskmgr.exe";
    auto res = check_output(cmd);
    MESSAGE(ed, "Shell Command: %s (exit-status=%d)\nOutput: %s",
            cmd.c_str(), res.status, res.output.c_str());
  });

/**
 * @page editor_ops
 * @section dns
 * Performs a DNS lookup for the given URL and prints the result on the command
 * status bar.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(DNSLookup, "dns", "editor_ops", DEF_OP() {
    std::string url = ed.prompt("URL: ");
    if (url.empty()) return;
    std::string res;
    try {
      res = dnsLookup(url);
    } catch (const std::runtime_error& e) {
      res = e.what();
    } catch (...) {
      res = "Unknown exception occured!";
    }
    CMBAR_MSG(ed, "%s\n", res.c_str());
  });

} // end namespace ops
} // end namespace editor
} // end namespace teditor
