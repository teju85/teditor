@mainpage teditor

@section mainpage_intro Introduction
teditor is a terminal based editor written from scratch, completely in C++.

@section mainpage_platforms Supported Platforms
This has currently been only tested on the following platforms:
1. Cygwin + mintty + xterm-256color
2. WSL + cmd-prompt + xterm-256color

That said, it should work for unix-based systems. If it doesn't build/work
properly on them, PR's or filing issues are most welcome.

@section mainpage_deps Dependencies
Very minimal set of pre-reqs are needed for this project.

@subsection mainpage_deps_compile Compile-time
1. GNU-make (>= 4.2.1)
2. G++ (>= 5.4.0)
3. doxygen (>= 1.8.13) - optional, only for documentation generation
4. curl - for downloading the Catch2 header file during unit-testing

@subsection mainpage_deps_run Runtime
1. curl - for downloading from web
2. git - to run git commands from inside the editor
3. infocmp - to get to know terminal capabilities
4. mintty - for running the editor
5. ssh - for launching remote commands

@section mainpage_usage Usage

@subsection mainpage_usage_source Build from source
```bash
git clone https://github.com/teju85/teditor
cd teditor
make -j teditor
./bin/Release/teditor
```

NOTE: If you have customized your minttyrc for the user configured colors, then
your experience with the default colors may vary!!

@subsection mainpage_usage_bin Pre-built binaries
For the above mentioned platforms, pre-built binaries are found here: TBD!

@subsection main_page_usage_tutorials Tutorials
Refer to @ref tutorials.

@section mainpage_dev Developers
Refer to @ref developers.

@section mainpage_release Releases
Refer to @ref releases.
