OpenZone
========

Simple cross-platform FPS/RTS game engine.

    http://ducakar.github.com/openzone/

This README file is intended for developers only. See `doc/README.html` if you are an end-user.

Building
--------

Building is currently supported under Linux. You can build Linux/Unix, Windows (MinGW) and Native
Client ports. Android port in still under development. See `cmake/*.Toolchain.cmake` files for all
supported platforms/toolchains.

For generic Linux builds, make sure you have all the dependencies installed. You need to install
development packages for the following libraries:

- ALSA (Linux only)
- libpulse (Linux/Unix only)
- PhysicsFS 2.0 or 2.1
- ODE (optional)
- Lua 5.1 or 5.2 or LuaJIT 2.0
- SDL 1.2 or 2.0
- SDL_ttf
- OpenGL 2.1+ or OpenGL ES 2.0
- OpenAL 1.1
- libvorbis
- FreeImage
- libsquish (optional)

You can then use generic steps for building CMake projects:

    mkdir build
    cd build
    cmake ..
    make

For a user-friendly GUI where you can configure build options, use `cmake-gui` instead of `cmake`.

For building all supported configurations you can use `ports.sh` and `build.sh` scripts. `ports.sh`
(see Tools section) downloads and builds all required libraries for NaCl and Android platforms
(NaCL SDK and Android SDK + NDK are required for this, of course), while `build.sh` builds OpenZone
for all platforms. You can change variables in headers of both scripts to change list of enabled
platforms and whether you want to make debug or a release build. You will also need to fix paths to
SDKs in those two scripts and in `cmake/*.Toolchain.cmake` files.

For building Linux and Windows builds, all required libraries need to be installed on your system.
MinGW32 is searched in `/usr/i486-mingw32` by default. You may change that in
`cmake/Windows-i686.Toolchain.cmake`.

You may also want to set several options when configuring CMake build system:

#### ozCore Library ####

- `OZ_TRACK_ALLOCS`: Enable tracking of allocated memory chunks in liboz. Stack trace for every
  memory allocation performed via new operator is saved for later diagnostics. It detects new/delete
  mismatches and one can check for currently allocated memory chunks (and hence memory leaks).
  `OFF` by default.

- `OZ_SIMD_MATH`: Enable SIMD-specific implementation of linear algebra classes (Vec3, Vec4, Point,
  Plane, Quat, Mat44). Currently it yields ~15% worse performance than generic implementation since
  Vec3 and Point classes are a bit larger (4 floats v. 3 floats) and there are plenty of accesses to
  vector components in OpenZone code.
  `OFF` by default.

#### ozDynamics Library ####

- `OZ_DYNAMICS`: Build complete OpenZone Dynamics Library. Requires ODE (Open Dynamics Engine)
  compiled in single precision. If turned off, ozDynamics is built only partially. OpenZone does not
  need ozDynamics library to run, only a few headers are required to build OpenZone.
  `OFF` by default.

#### OpenZone Engine ####

- `OZ_LUAJIT`: Use LuaJIT instead of official Lua library. Lua scripts execute significantly faster.
  `OFF` by default.

- `OZ_SDL2`: Use upcoming SDL 2.0 instead of stable SDL 1.2.
  `OFF` by default, forced to `ON` on Android, forced to `OFF` on NaCl.

- `OZ_NET`: Enable networking support (not imelemented yet). Requires SDL_net library.
  `OFF` by default, forced to `OFF` on NaCl.

- `OZ_GL_ES`: Use OpenGL ES 2.0 API. Enabling this on Linux or Windows while using SDL 1.2 leads to
  a strange situation when SDL initialises OpenGL but rendering is done entirely through OpenGL ES.
  However, it seems to work on Linux at least.
  `OFF` by default, forced to `ON` on Android and NaCl.

#### ozBuild Tool ####

- `OZ_NONFREE`: Enable support for building textures using S3 texture compression.
  Requires libsquish library.
  `OFF` by default.

#### Packaging ####

- `OZ_STANDALONE`: This only affects behaviour of "`make install`". It also installs dependencies
  from support directory, game data archives found in `share/openzone`, info files etc. This is
  intended if one wants to create all-in-one ZIP (or whatever) archive that can be unpacked and run
  on any Linux distro or Windows without installation.
  `OFF` by default, forced to `ON` on Windows, forced to `OFF` on Android and NaCl.

Tools
-----

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
temporary directory and put into newly built package. So, it is highly recommended to remove all
temporary directories ("`rm -rf share/openzone/*`" on Linux) after removing something from game data
or before doing the final build.

### `ozManifest` ###

This tool creates package manifest file required by NaCl port for updating game data packages. It
searches for all game data packages (`*.7z` and `*.zip`) in `share/openzone` directory (first
parameter is used as target directory, `share/openzone` if nothing is given) and writes manifest
file `share/openzone/packeages.ozManifest` containing list of package files and respective
timestamps.

Scripts
-------

The following helper scripts are located in the root directory of the source tree.

### `autogen.sh` ###

Some targets have lots of source files and it would be hard to keep them all consistent and
up-to-date manually. Lists of source files in corresponding `CMakeLists.txt` are thus generated by
this script.

Additionally this scripts updates version numbers in various files.

### `build.sh [clean | conf | build]` ###

This script configures and/or builds OpenZone for all supported platforms in the `build` directory.
`ANDROID_NDK` and `NACL_SDK_ROOT` environment variables must be set to use this script.

The following commands may be given (`build` is assumed if none):

- `clean`: Delete all builds.
- `conf`: Delete all builds and configure (but not build) them anew.
- `build`: Configure (if necessary) and build all builds.
- `pnacl`: Run `pnacl-translate` to convert client `.pexe` to platform-dependent `.nexe`s.

### `capture.sh` ###

Run OpenZone with glc tool to capture video. Audio is captured from PulseAudio server and saved
into `openzone-pa.wav` file.

### `clean-blanks.sh` ###

Cleans up trailing blanks, duplicated empty lines and missing newlines at the end of file for most
files in the source tree.

### `count.sh` ###

Prints SLOC (source lines of code) statistics generated by SLOCCount and cloc (if found installed on
the system).

### `cppcheck.sh` ###

Runs cppcheck tool for static code analysis. Output is written into `cppcheck.log`.

### `data-cleanup.sh <datasrc_dir>` ###

Cleans up temporary, intermediate and backup files from a source game data directory.

### `gettext-base.sh <basedata_dir>` ###

Creates `<basedata_dir>/<basename>.pot` template for localised messages catalogue. See `ozGettext`
tool for more details. `<basename>` is package name (last directory name in `<basedata_dir>`).

### `lib.sh [clean | build]` ###

Copy all libraries OpenZone depends on to `lib/<platform>` directories (currently Linux-x86_64,
Linux-i686 and Windows-i686). Those are required to create standalone build (see OZ_STANDALONE cmake
option) that can be distributed in a ZIP archive (i.e. no installation required, all dependencies
included).

This script is currently Arch Linux-specific and assumes one has all 64-bit, 32-bit (`lib32-*`) and
MinGW (`mingw32-*`) versions of all required libraries installed. Many of those packages must be
built from AUR.

The following commands may be given (`build` is assumed if none):

- `clean`: Delete directories for all platforms.
- `build`: Copy libraries for selected platforms into corresponding directories.

### `package.sh {src | data | datasrc | boundle}` ###

One of the following commands must be given:

- `src`: Create source archive `openzone-src-<version>.tar.xz`.
- `data`: Create compiled data archive `openzone-data-<version>.tar.xz`. All data packages found in
  `share/openzone` directory are included.
- `datasrc`: Create source data archive `openzone-data-src-<version>.tar.xz`. All source data
  packages found in `data` directory are included.
- `boundle`: Create a ZIP archive that includes Linux-x86_64, Linux-i686 and Windows-i686 standalone
  builds and compiled game data packages found in `share/openzone`.

### `ports.sh [clean | fetch | build]` ###

This script is used to build libraries required by OpenZone for some platforms. Currently it builds
all required libraries for NaCl and Android configurations that are not provided by SDKs.
`ANDROID_NDK` and `NACL_SDK_ROOT` environment variables must be set to use this script.

The following commands may be given (`build` is assumed if none):

- `clean`: Delete everything in `ports` directory except downloaded sources.
- `buildclean`: Delete build directories. Downloaded sources and installed libraries are left
  intact.
- `fetch`: Download sources into `ports/archives` directory.
- `build`: Builds all libraries for all platforms.

### `q3map2.sh <map_file>` ###

Proxy script for invoking `q3map2` BSP compiler from GtkRadiant in with appropriate parameters to
compile a BSP structures for OpenZone.

### `run.sh [win | nacl] [<options>]` ###

Linux-x86_64-Clang client is launched by default. <options> are passed to the client command line.
`NACL_SDK_ROOT` environment variable must be set to use this script.

The following alternative launches are available:

- `wine`: Installs the standalone Windows port into `build/Windows-test` and launches it via Wine.
- `nacl`: Prepares directory `build/NaCl-test` (creating symlinks to binaries, compiled data
  archives, HTML pages ...), starts a simple python web server in that directory at port 8000
  (`python -m http.server`) and opens `localhost:8000` in chromium browser to test the web-based
  NaCl port. If `strip` is passed as the second option, binaries are stripped too.
- `pnacl`: Same as `nacl` command but create links `.nexe`s that were created from a `.pexe`
  (use `./build.sh pnacl`).

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
version. liboz library is a part of OpenZone engine but can also be distributed or modified under
ZLIB licence.

Game data are obtained from various sources and covered by many different licences. See individual
README.txt and COPYING.txt files inside game data archives for details.

This program comes with ABSOLUTELY NO WARRANTY.
