# teditor
*teditor* is a terminal based editor, written from scratch in C++. More details
about this editor can be found [here](https://teju85.github.io/blog/tags.html#teditor).

# Supported OS's
This has been only tested on cygwin with mintty and xterm-256 till now! Should
work for unix-based systems. If it doesn't build/work properly on them, fixes or
filing issues are most welcome.

# Pre-reqs
Very minimal set of pre-reqs are needed for this project.

## Compile time
1. GNU-make (>= 4.2.1)
2. G++ (>= 5.4.0)
3. doxygen (>= 1.8.13) - for documentation generation
4. curl - for downloading Catch2 header

## Runtime (alphabetical order)
1. curl - for downloading from web
2. git - to run git commands from inside the editor
3. infocmp - to get to know terminal capabilities
4. mintty - for running the editor
5. ssh - for launching remote commands

# Download and Build
```bash
git clone https://github.com/teju85/teditor
cd teditor
make -j teditor
./bin/Release/teditor
```
NOTE: If you have customized your minttyrc for the user configured colors,
then your experience with the default colors may vary!!

# Developers
## Generating documentation
It is expected that one has installed doxygen for this purpose. Generated
documentation can then be found inside the folder 'html'.
```bash
make doc
```

## Unit testing
Unit-tests are run via catch2.
```bash
make -j tests
```

## Debug builds
```bash
make DEBUG=1 -j teditor tests
```
