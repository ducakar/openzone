![logo](http://ducakar.github.io/openzone-web/img/title.jpg)

OpenZone
========

Simple cross-platform FPS/RTS game engine. http://ducakar.github.io/openzone/.

Running
-------

For standalone packages, run `bin/<platform>/openzone` executable to start the game, where
`<platform>` is your operating system-instruction set pair.

Building
--------

Building is currently only supported under Linux. You can build Linux/Unix, Windows (MinGW) and
Native Client ports. Android port in still under development. See `cmake/*.Toolchain.cmake` files
for all supported platforms/toolchains.
GCC >= 5 and LLVM/Clang >= 3.4 are the only supported compilers.

### Dependencies ###

Development packages of the following libraries are required to build OpenZone from source:

- ALSA (Linux only)
- libpng
- libvorbis
- Lua or LuaJIT
- openal-soft
- OpenGL or OpenGL ES
- opusfile
- PhysicsFS
- SDL2
- SDL2_ttf
- zlib

If you want to build the tools (`OZ_TOOLS` option) you also need:

- Assimp
- FreeImage
- libnoise
- libsquish

The following development tools are required:

- CMake
- Ninja (optional, recommended)
- LLVM/Clang (optional, recommended)
- MinGW-w64 (optional, for building Windows-i686 and Windows-x86_64 ports)
- NaCl SDK (optional, for building PNaCl port)

### Quick Build Instructions ###

Clone the repository:

    git clone --recursive https://github.com/ducakar/openzone.git
    cd openzone

You can then use generic steps for building CMake projects:

    mkdir -p build && cd build
    cmake ..
    make
    cd ..

And then compile the game data:

    ./build/src/tools/ozBuild -CAZ data/oz_base
    ./build/src/tools/ozBuild -CAZ data/oz_main
    ./build/src/tools/ozBuild -CAZ data/oz_missions

After that the game is prepared for a test run:

    ./build/src/tools/openzone -p .

### Long Build Instructions ###

For building all supported configurations you can use `ports.sh` and `build.sh` scripts. `ports.sh`
(see Tools section) downloads and builds all required libraries for NaCl and Android platforms
(NaCL SDK and Android SDK + NDK are required for this, of course), while `build.sh` builds OpenZone
for all platforms. You can change variables at top of both scripts to change the list of enabled
platforms and whether you want to make debug or a release build.

Build scripts use Ninja as low-level build system instead of Make which is the default for CMake.

For building Linux and Windows builds, all required libraries need to be installed on your system.
MinGW64 is searched for in `/usr/i686-w64-mingw32` and `/usr/x86_64-w64-mingw32` by default. You may
change that in `cmake/Windows-*.Toolchain.cmake`.

You may also want to adjust several options when configuring CMake build system:

- `OZ_ALLOCATOR`: Enable memory allocation statistics and tracking of allocated memory chunks. Stack
  trace for every memory allocation performed via new operator is saved for later diagnostics. It
  detects new/delete mismatches and can be used to list the currently allocated memory chunks (and
  hence memory leaks). Upon freeing, a memory chunk is rewritten with 0xee bytes which makes
  accesses to the freed memory very likely to result in an error or a crash.
  `OFF` by default.

- `OZ_SIMD`: Enable SIMD-specific implementation (SSE1 & ARM NEON) of linear algebra classes (Vec3,
  Vec4, Point, Plane, Quat, Mat3, Mat4). Currently it yields ~15% worse performance than the generic
  implementation since `Vec3` and `Point` become longer (4 floats v. 3 floats) and there are plenty
  of accesses to vector components in the code.
  `OFF` by default.

- `OZ_GL_ES`: Use OpenGL ES 2.0 instead of OpenGL 2.1.
  `OFF` by default, forced to `ON` on Android and NaCl.

- `OZ_LUAJIT`: Use LuaJIT instead of the official Lua library. Lua scripts execute much faster but
  LuaJIT is written in assembler and supported only on x86 desktop platforms.
  `OFF` by default.

- `OZ_TOOLS`: Build tools required for building game data (see the next section).
  `OFF` by default.

- `OZ_TESTS`: Build liboz unittest and various experimental executables used as a playground when
  developing OpenZone. You don't need this.
  `OFF` by default.

Tools
-----

### `ozBuild -CAZ <pkgSrc>` ###

Compiler for game data. It compiler source game data into formats more suitable for OpenZone engine
which results in much better run-time performance and much less code required to load the data.

Data sources are read from `<pkgSrc>` directory. Output directory is `share/openzone` by default
(you can specify it as an additional parameter after `<pkgSrc>` if you wish otherwise). Last token
from `<pkgSrc>` path is interpreted as the package name. The package is built inside
`share/openzone/<pkgName>` directory and the game data archive is packed after successful build in
`share/openzone/<pkgName>.zip` if run with `-A` parameter. See "`ozBuild --help`" for all options.

Note that temporary directory `share/openzone/<pkgName>` is not cleared after build, so if you
remove any files from source data and rebuild package, removed files will still be cached in the
temporary directory and included into newly built package. Hence it's highly recommended to remove
all temporary directories ("`rm -rf share/openzone/*`" on Linux) after removing something from the
game data or before doing the final build.

### `ozGenEnvMap -CM` ###

This tool was created to generate environment map for reflections (`oz_base.zip/glsl/env.dds`).

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

Scripts
-------

The following helper scripts are located in the root directory of the source tree.

### `autogen.sh` ###

Some targets have lots of source files and it would be hard to keep them all consistent and
up-to-date manually. Lists of source files in corresponding `CMakeLists.txt` are thus generated by
this script.

Additionally this scripts updates version numbers in various files.

### `build.sh [clean | conf] [<platform>]` ###

This script configures and/or builds OpenZone in the `build` directory for a specified platform or
all supported platforms uncommented in the beginning of this script if `<platform>` parameter is
omitted. `ANDROID_NDK` and `NACL_SDK_ROOT` environment variables must be set for Android and NaCl
builds respectively.

The following commands may be given:

- `clean`: Delete build(s).
- `conf`: Delete build(s) and configure (but not build) them anew.
- (none): Configure (if necessary) and build.

### `capture.sh` ###

Run OpenZone with glc tool to capture video. Audio is captured from PulseAudio server and saved
into `openzone-pa.wav` file.

### `clean-blanks.sh` ###

Cleans up trailing blanks, duplicated empty lines and missing newlines at the end of files in the
source tree.

### `count.sh` ###

Prints SLOC (source lines of code) statistics generated by SLOCCount and cloc (if found installed on
the system).

### `cppcheck.sh` ###

Runs cppcheck tool for static code analysis. Output is written into `cppcheck.log`.

### `data-cleanup.sh <datasrc_dir>` ###

Cleans up temporary, intermediate and backup files from a game source data directory.

### `gettext-internal.sh <data_dir>` ###

Extracts internal strings for localisation from OpenZone source code and appends them to
`<data_dir>/lingua/<data_dir_basename>.pot`.

### `lib.sh [clean]` ###

Copy all libraries OpenZone depends on to `lib/<platform>` directories (currently Linux-x86_64,
Linux-i686 and Windows-i686). Those are required to create standalone build (see `OZ_BUNDLE` cmake
option) that can be distributed in a ZIP archive (i.e. no installation required, all dependencies
included).

This script is currently Arch Linux-specific and assumes one has all 64-bit, 32-bit (`lib32-*`) and
MinGW (`mingw32-*`) versions of all necessary libraries installed. Many of those packages must be
built from AUR.

The following commands may be given (`build` is assumed if none):

- `clean`: Delete directories for all platforms.
- (none): Copy libraries for selected platforms into corresponding directories.

### `nacl.sh [run | finalise]` ###

`NACL_SDK_ROOT` environment variable must be set to use this script.

The following alternative launches are available:

- `run`: creates symlinks to compiled data archives, HTML pages etc. in the target directory, starts
  a simple python web server in that directory at port 8000 (`python -m http.server`) and opens
  `localhost:8000` in chromium browser to test the web-based NaCl port.
- `manifest`: writes `share/openzone/manifest.json` file that contains list of game packeges
  together with their timestamps. Needed by NaCl to update cached game packages.

### `package.sh {src | data | datasrc | bundle}` ###

One of the following commands must be given:

- `src`: Create source archive `openzone-src-<version>.tar.xz`.
- `data`: Create compiled data archive `openzone-data-<version>.tar.xz`. All data packages found in
  `share/openzone` directory are included.
- `datasrc`: Create source data archive `openzone-datasrc-<version>.tar.xz`. All source data
  packages found in `data` directory are included.
- `bundle`: Create a 7zip archive that contains Linux-x86_64, Linux-i686 and Windows-i686 standalone
  builds and compiled game data packages found in `share/openzone`.

### `ports.sh [clean | buildclean | fetch]` ###

This script is used to build libraries required by OpenZone for some platforms. Currently it builds
all required libraries for NaCl and Android configurations that are not provided by SDKs.
`ANDROID_NDK` and `NACL_SDK_ROOT` environment variables must be set to use this script.

The following commands may be given (`build` is assumed if none):

- `clean`: Delete everything in `ports` directory except downloaded sources.
- `buildclean`: Delete build directories. Downloaded sources and installed libraries are left
  intact.
- `fetch`: Download sources into `ports/archives` directory.
- (none): Builds all libraries for all platforms.

### `q3map2.sh <map_file>` ###

Proxy script for invoking `q3map2` BSP compiler from GtkRadiant in with appropriate parameters to
compile a BSP structures for OpenZone.

### `run.sh [wine | wine64] [<openzoneOptions>]` ###

Linux-x86_64-Clang client is launched by default. <options> are passed to the client command line.
The following alternative launches are available:

- `wine`: Installs and launches standalone Windows i686 port via Wine.
- `wine64`: Installs and launches standalone Windows x86-64 port via Wine.

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

OpenZone engine is licensed under GNU General Public License version 3 or (at your option) any later
version (see `COPYING` file). liboz library (`src/ozCore`, `src/ozEngine` and `src/ozFactory`
directories) is a part of OpenZone engine but can also be distributed or modified separately under
the zlib licence (see `src/ozCore/COPYING`).

Libraries that may be bundled with binary distributions of OpenZone come under different licences.
See `doc/README.html` for details.

Game data are obtained from various sources and covered by many different licences. See individual
`README.txt` and `COPYING.txt` files inside game data archives for details.

This program comes with ABSOLUTELY NO WARRANTY.
