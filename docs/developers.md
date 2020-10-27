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
1. Check that the `VERSION` variable inside `Makefile` is updated properly.
2. Create a `docs/releases/v$(VERSION).md` file and update its contents with the
   list of features in the current release. You can copy the contents of another
   file in this dir and use it as a starting point.
3. Update `docs/releases.md` to point to the document of this new release.
4. Commit all changes if not done already.
5. For every supported release platform:
   5a. Run `make release`. It creates a `teditor-<version>/<OS>` folder.
6. Run the command: `make package`, which converts the `teditor-<version>` into
   a tarball with the same basename.
7. Push all the changes to remote: `git push origin master`
8. Go to [releases](https://github.com/teju85/teditor/releases/new) page and
   prepare a new release. Attach the above tarball into this page.
9. After verifying the contents of this page, click `Publish release`.
