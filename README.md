This README file is intended for developers only. See `doc/README.html` if you are an end-user.

Building
--------

Building is currently only supported on Linux.

You can use generic steps for building CMake projects. Basic build instructions when you are in the
root directory of the source tree are:

        mkdir build
        cd build
        cmake ..
        make

For building all supported configurations you can use `configure-all.sh` and `build-all.sh` scripts.
For this step you will need a 64-bit Linux distro and all dependencies installed (including 32-bit
and MinGW32 version of all libraries that OpenZone depends on, MinGW root is `/usr/i486-mingw32` by
default, you can change it in ``cmake/MinGW32.Toolchain.cmake`).

You may also want to set several options when configuring CMake build system:

* `OZ_SHARED_LIBOZ`: Build liboz (OpenZone core library) as a shared library. This is useful if one
  wants to put liboz into a separate Linux package to use it by other projects.

* `OZ_TRACE_LEAKS`: whether to enable tracking of allocated memory chunks (including stack trace
  from where allocations were performed. This can provide nice diagnostics for new/delete mismatches
  and memory leaks). Off by default.

* `OZ_GL_ES`: Use OpenGL ES 2.0 API. Always enabled in NaCl platform, on other platforms it will
  merely use GL ES headers and functions set, it will still initialise OpenGL.

* `OZ_NONFREE`: Enable support for decoding MP3 and AAC formats. Required libraries (libmad and
  faad2) are loaded in run-time if found on system or library search path.

* `OZ_STANDALONE`: This only affects behaviour of "`make install`". It also installs dependencies
  from support directory, game data archives found in `share/openzone`, info files etc. This is
  intended if one wants to create all-in-one ZIP (or whatever) archive that can be unpacked and run
  on any Linux distro or Windows without installation. Always on for Windows build, otherwise off by
  default.


Tools
-----

### `ozSineSample` ###

Simple tool that I use to generate sound sample that goes into `oz/src/bellSample.inc` file. It
takes no arguments (everything must be changed in `src/tools/ozSineSample.cc`) and writes array of
samples to stdout.

### `ozGettext <pkgSrc>` ###

It extracts to-be-localised strings from (source) game data and creates corresponding POT catalogue
templates. That can be used to create new catalogues or update existing ones with standard
gettext-based tools (e.g. Poedit).

Strings that are extracted for localisation are full contents of credits file
(`<pkgSrc>/credits/<pkgName>.txt`) and names of BSPs and BPS models models
(in `<pkgSrc>/baseq3/maps/*.json`) and object classes (in `<pkgSrc>/class/*.json`). Those are
written into `<pkgSrc>/lingua/<pkgName>/*.pot` catalogue template. Catalogues with translations
should be saved as `<pkgSrc>/lingua/<lang>/<pkgName>.po`, where `<lang>` is Linux language code
(usually first two letters in locale name, e.g. "sl" for "sl_SI.UTF-8" locale. Your current locale
on a Linux system is given in `LANG` and/or `LANGUAGE` environment variables).

For missions, strings are extracted from descriptions (in `<pkgSrc>/mission/*/description.json`),
cinematic sequence titles (in `<pkgSrc>/mission/*/*.sequence.json`) and from mission scripts
(parameters of `ozGettext` functions found in `<pkgSrc>/mission/*/*.lua`). Per-mission translation
catalogue is written into `<pkgSrc>/mission/<missionName>/lingua/messages.pot`. Message catalogue
should be saved as `<pkgSrc>/mission/<missionName>/lingua/<lang>.po`.

Note that ozbase is a special package and its POT catalogue template should be generated with
gettext-ozbase.sh Bash script instead.

### `ozBuild -A <pkgSrc>` ###

Compiler for game data. It compiler source game data into formats more suitable for OpenZone engine
which results in much better run-time performance and much less code required to load data.

Data source are read from `<pkgSrc>` directory. Output directory is `share/openzone` by default (you
can specify it as an additional parameter after `<pkgSrc>` if you wish otherwise). Last token from
`<pkgSrc>` path is interpreted as package name. Package is built in `share/openzone/<pkgName>`
directory and game data archive is packed after successful build in `share/openzone/<pkgName>.zip`,
if run with `-A` parameter. See "`ozBuild --help`" for all options.

Note that temporary directory `share/openzone/<pkgName>` is not cleared after build, so if you
remove any files from source data and rebuild package, removed files will still be cached in
temporary directory and put into newly build package. So, it is advisable to remove all temporary
directories ("`rm -rf share/openzone/*`" on Linux) after removing something from game data or before
doing final builds.

### ozManifest ###

This tool creates package manifest file required by NaCl port for updating game data packages. It
searches for all game data packages (`*.7z` and `*.zip`) in `share/openzone` directory (first
parameter is used as target directory, `share/openzone` if nothing is given) and writes manifest
file `share/openzone/packeages.ozManifest` containing list of package files and respective
timestamps.


Scripts
-------

The following scripts are located in root directory of source tree.

### `autogen.sh` ###

Generate some `CMakeLists.txt` files and update OpenZone version in several files. Some components
of OpenZone have many source files and keeping all `CMakeLists.txt` files up-to-date by hand
wouldn't be easy.

### `build-all.sh` ###

Runs compilation of all build configurations. See `configure-all.sh` for details.

### `check.sh` ###

Runs cppcheck tool for static code analysis.

### `clean-blanks.sh` ###

Cleans up trailing blanks, duplicated empty lines and missing newlines at the end of file for most
files in the source tree.

### `configure-all.sh` ###

Creates and configures all build configurations inside `build` directory. Fix variables at the
beginning of this script to enable/disable specific configuration.

### `count.sh` ###

Prints SLOC (source lines of code) statistics generated by SLOCCount and CLOC (if found installed
int the system).

### `gettext-ozbase.sh` ###

Creates `data/ozbase/lingua/ozbase.pot` template for localised messages catalogue. See `ozGettext`
tool for more details.

### `nacl-test.sh` ###

Prepares directory `build/NaCl-test` (creating symlinks to binaries, compiled data archives, HTML
pages ...) for web-based, NaCl port, and launches chromium browser at `localhost:8000`. It assumes
one has already a simple web server running at port 8000, serving `build/NaCl-test` directory (e.g.
go to `build/NaCl-test` and run "`python -m http.server`").

### `pack-src.sh` ###

Pack source tree into `openzone-src-<version>.tar.xz` and pack compiled game data archives from
`share/*.zip` into `openzone-data-<version>.tar.xz`.

### `support-all.sh` ###

Copy all libraries OpenZone depends on to `support/<platform>` directories. Those are required to
create standalone build (cmake OZ_STANDALONE option) that can be distributed in a ZIP archive (i.e.
no installation required, all dependencies included).

This script is currently Arch Linux-specific and assumes one has all 64-bit, 32-bit (`lib32-*`) and
MinGW (`mingw32-*`) versions of all dependent libraries installed. Many those packages must be built
from AUR.


Documentation
-------------

To generate documentation from source code with Doxygen, run

        doxygen doc/Doxyfile

or, if you want to generate documentation for liboz only, run

        doxygen doc/Doxyfile.liboz

from source tree root. Open `doc/doxygen/html/index.html` or `doc/doxygen.liboz/html/index.html` to
see documentation once it is generated.

Currently only liboz documentation is complete.


Legal
-----

OpenZone engine is written by Davorin Učakar and licensed under GNU GPL 3.0.

Game data archives and libraries distributed with OpenZone are work of various authors and use
separate licences. For more details see `doc/README.html` for library licences and files named
`README.txt` and `COPYING.txt` inside game data archives for game data licences.

This program comes with ABSOLUTELY NO WARRANTY.
