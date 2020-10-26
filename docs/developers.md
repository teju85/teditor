@page developers Information for developers

@section developers_doc Generating documentation
It is expected that one has installed `doxygen` for this purpose. Generated
documentation can then be found inside the folder `bin/html`.
```bash
make doc
```

@section developers_tests Unit testing
Unit-tests are run via catch2.
```bash
make -j tests
```

@section developers_debug_builds Debug builds
```bash
make DEBUG=1 -j teditor tests
./bin/Debug/teditor
```
In addition for debug symbols, this also enables `DEBUG` and `ULTRA_DEBUG`
logger macros in the logging module which can be very helpful while debugging
editor core modules.

@section developers_release_checklist Release Checklist
1. Check that the `VERSION` variable inside `Makefile` is updated properly
2. Run `make release`
