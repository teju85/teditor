@page releases Releases

@section releases_170 v1.7.0
Date: 2020-09-27

Glad to announce the release of the next version of teditor: v1.7.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.7.0).

@subsection releases_170_changes Changes
Changes/features in this release:
1. Command for DNS lookup: `M-x dns`.
2. In-built calculator: `M-x calc`.
3. Command to open files using another programs while in dir-mode. Just hit `s`
   while putting point on that file.
4. Show the hits nearby the current point location itself during `search`.
5. Removed pcre2 and catch2 as submodules (helps reduce repo size). From now
   onwards, `parser::NFA` will be used for regex's instead of pcre2.
6. Option for debug and release builds in the build infra.
7. Ability to filter irrelevent commands in any mode.
8. A `History` base class for storing historical commands, files visited, etc
9. ledger-mode now has a stronger parser based on Lexing!
10. A `LRUCache` class


@section releases_160 v1.6.0
Date: 2019-11-30

Glad to announce the release of the next version of teditor: v1.6.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.6.0).

@subsection releases_160_changes Changes
Changes/features in this release:
1. grep mode
2. Support for working with remote files
3. Simplified indent/unindent logic
4. File history storage syntax update in light of remote file syntax
5. Enabled keep-lines/remove-lines command and also their undo/redo
6. Alert users about working on remote buffers
7. Case insensitive searching via a rc option
8. Various fixes to improve perf on remote buffers (eg: usage of DirCache)
9. Various bug fixes and addition of utility methods

@subsection releases_160_grep grep mode
This is similar to the grep mode in emacs. `M-x grep <enter>` type the regex
to search for and the file/dir where to search and then press enter. This should
open a buffer with all grep results highlighted. From then onwards, pressing
enter on each result of grep will take you to that file and line (similar to
what happens in grep mode of emacs).


@section releases_150 v1.5.0
Date: 2019-11-19

Glad to announce the release of the next version of teditor: v1.5.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.5.0).

@subsection releases_150_changes Changes
Changes/features in this release:
1. Brand new Option interface to customize editor behavior via rc-file
2. Revamp of commandline parsing to print help message, pass rc-file, etc
3. Track version info from the commandline
4. Bug fixes to removeRegion functionality
5. Support in dir-mode for opening remote directories
6. Reorg of cmds and modes source files to something more aptly named locations

@subsection releases_150_cmdline Commandline options
From now onwards, commandline options for the editor are documented and can be
accessed via the following command: `teditor -h`

@subsection releases_150_rc Customizing editor via rc files
Easiest way is to create a fresh rc file via the `-dump` option and then edit it
to customize as per your needs.
```bash
teditor -dump /path/to/rc/file
# open and edit this file
```


@section releases_140 v1.4.0
Date: 2019-11-18

Glad to announce the release of the next version of teditor: v1.4.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.4.0).

@subsection releases_140_changes Changes
Changes/features in this release:
1. Complete removal of multiple-cursors from the editor. (reason below)
2. Bug fixes to highlighting the empty lines under selection.

@subsection releases_140_no_mc Why no Multiple Cursors?
There are multiple reasons for this:
1. We'll introduce more macro-like interface that'll void the need for such a
   thing. Many blogs have been written about this [here](https://medium.com/@schtoeffel/you-don-t-need-more-than-one-cursor-in-vim-2c44117d51db)
   and [here](https://nullprogram.com/blog/2017/09/07/). Thus I do not want to
   elaborate further on this.
2. In future, underlying implementation of buffers can be optimized very easily
   without multiple-cursors support [Reference](https://nullprogram.com/blog/2017/09/07/)


@section releases_130 v1.3.0
Date: 2019-08-11

Glad to announce the release of the next version of teditor: v1.3.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.3.0).

@subsection releases_130_changes Changes
Changes/features in this release:
1. Adds support for ledger-like text file based syntax for personal finanace
   accounting.
2. Reintroduces proper kill-line command based on the new undo/redo stack.
3. More unittests improve coverage
4. Bug fixes to the undo/redo for insert-char command.

### Ledger Mode
To start using this for tracking your personal finances:
1. Create a file named `ledger.lg` inside your `~/.teditor` folder. (one can
   also create such a file elsewhere and then use the `-ledger-file` option)
2. Refer to the file `samples/ledger/sample.lg` to know its syntax. It is very
   simplified version of the [ledger-cli](https://www.ledger-cli.org/) syntax.
3. Once you have this file populated with information, get back to the teditor
   window and press `C-l C-d` in order to see the summary of your transactions.
4. By default, it shows a summary report of all transactions grouped under the
   top-level accounts. Press `A` to show them for all accounts.


@section releases_120 v1.2.0
Date: 2019-08-07

Glad to announce the release of the next version of teditor: v1.2.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.2.0).

@subsection releases_120_changes Changes
Changes/features in this release:
1. Various fixes to the bugs in draw calls of the main window
2. Proper resizing now handled directly by Windows class, out of Buffer class
3. Full support for a vertical window split


@section releases_110 v1.1.0
Date: 2019-08-06

Glad to announce the release of the next version of teditor: v1.1.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.1.0).

@subsection releases_110_changes Changes
Changes/features in this release:
1. Initial set of code changes for an upcoming multiple windows support.
2. Proper wrapper for tree-sitter (soon to be used for code tree traversals)


@section releases_100 v1.0.0
Date: 2019-01-26

Today, I'm glad to announce the release of its next version: v1.0.0. It
can be found [here](https://github.com/teju85/teditor/releases/tag/1.0.0).

@subsection releases_100_changes Changes
Changes/features in this release:
1. Completely revamped command interface, so that users don't have explicitly
   encode undo/redo commands. This makes writing of commands/extensions much
   easier.
2. Every new command can be made to self-document and now there's a help command
   to query just that!
3. Fixes to many bugs in editing commands, especially with active regions.


@section releases_010 v0.1.0
Date: 2018-06-24

Teditor is a terminal based editor completely written in C++. It is inspired
from [eon](https://github.com/tomas/eon) and
[emacs](https://www.gnu.org/software/emacs/). The project is hosted
[here](https://github.com/teju85/teditor).

@subsection releases_010_yet Yet another editor!?
While all editors support terminal capabilities, how about we try to have a
terminal support editor capabilities!? teditor is a terminal based editor,
written in C++, to explore this aspect.

Besides, writing an editor from scratch is a real practical way to get to know a
lot about programming, SW life cycle management, OS and terminal concepts!

@subsection releases_010_changes Changes
Current release of teditor is at v0.1.0. It can be found
[here](https://github.com/teju85/teditor/releases/tag/0.1.0). Features in this
release:
1. Basic editing features
2. Support for multiple buffers and switching between them
3. 'Command' API to extend functionality
4. Support for dir-mode; just like in emacs
5. Region selection
6. Support for multiple-cursors
7. Sorting a selected region in the buffer
8. keep-lines, remove-lines; just like in emacs
9. M-x command; just like in emacs
10. text search and potentially use it as a way to quickly navigate inside the
    buffer
11. Stores files opened history for a quicker navigation later
