OpenZone
========

Simple cross-platform FPS/RTS game engine.

    http://ducakar.github.com/openzone/

This README file is intended for developers only. See `doc/README.html` if you are an end-user.

Building
--------

Building is currently only supported on Linux.

First, make sure you have all the dependencies installed. You need to install development packages
for the following libraries:

* ALSA
* PulseAudio client library (libpulse)
* PhysicsFS 2.0 or 2.1
* Lua 5.1 or 5.2
* SDL 1.2 or 2.0
* SDL_ttf
* OpenGL 2.1+ (Mesa headers) or OpenGL ES 2.0
* OpenAL 1.1
* libvorbis
* libmad (optional)
* faad (optional)
* eSpeak (optional)
* FreeImage
* libsquish (optional)

You can then use generic steps for building CMake projects. Basic build instructions when you are in
the root directory of the source tree are:

    mkdir build
    cd build
    cmake ..
    make

For building all supported configurations you can use `configure-all.sh` and `build-all.sh` scripts.
For this step you will need a 64-bit Linux distro and all dependencies installed (including 32-bit
and MinGW32 version of all libraries that OpenZone depends on, MinGW root is `/usr/i486-mingw32` by
default, you can change it in ``cmake/MinGW32.Toolchain.cmake`).

You may also want to set several options when configuring CMake build system:

* `OZ_SHARED_LIBS`: Build liboz (OpenZone Core Library) and libozdyn (OpenZone Dynamics Library) as
  shared libraries. This is useful if one wants to put liboz and libozdyn into separate Linux
  packages so other programs can use them too.

* `OZ_TRACK_ALLOCS`: Enable tracking of allocated memory chunks in liboz. Stack trace for every
  memory allocation performed via new operator is saved for later diagnostics. It detects new/delete
  mismatches and one can check for currently allocated memory chunks (and hence memory leaks).

* `OZ_SIMD_MATH`: Enable SIMD-specific implementation of linear algebra classes (Vec3, Vec4, Point,
  Plane, Quat, Mat44). Currently it yields ~15% worse performance than generic implementations since
  Vec3 and Point classes are a bit larger (4 floats v. 3 floats) and there are lots of accesses to
  vector components in OpenZone code.

* `OZ_GL_ES`: Use OpenGL ES 2.0 API. Always enabled in NaCl platform, on other platforms it will
  merely use GL ES headers and functions set, it will still initialise OpenGL.

* `OZ_NONFREE`: Enable support for decoding MP3 and AAC formats and building textures using S3
  texture compression. Requires libmad, faad and libsquish libraries.

* `OZ_STANDALONE`: This only affects behaviour of "`make install`". It also installs dependencies
  from support directory, game data archives found in `share/openzone`, info files etc. This is
  intended if one wants to create all-in-one ZIP (or whatever) archive that can be unpacked and run
  on any Linux distro or Windows without installation. Always on for Windows build, otherwise off by
  default.


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
temporary directory and put into newly build package. So, it is advisable to remove all temporary
directories ("`rm -rf share/openzone/*`" on Linux) after removing something from game data or before
doing final builds.

### `ozManifest` ###

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

### `build.sh` ###

Configures and/or builds for all supported platforms (see `platforms` array is the scrips) in the
`build` directory.
One of the following parameters must be given (`build` is assumed if run without parameters):

    * `clean`: delete all builds
    * `conf`: delete all builds and configure (but not build) them anew
    * `build`: configure (if necessary) and build all builds.

### `capture.sh` ###

Run OpenZone with glc tool to capture video. Audio is captured from PulseAudio server and saved
into `openzone-pa.wav` file.

### `check.sh` ###

Runs cppcheck tool for static code analysis.

### `clean-blanks.sh` ###

Cleans up trailing blanks, duplicated empty lines and missing newlines at the end of file for most
files in the source tree.

### `count.sh` ###

Prints SLOC (source lines of code) statistics generated by SLOCCount and CLOC (if found installed
int the system).

### `data-cleanup.sh` ###

Cleans up temporary and backup files from source game data directories in `data/` created by various
programs during editing those files.

### `gettext-ozbase.sh` ###

Creates `data/ozbase/lingua/ozbase.pot` template for localised messages catalogue. See `ozGettext`
tool for more details.

### `libs.sh` ###

Copy all libraries OpenZone depends on to `libs/<platform>` directories (currently Linux-x86_64,
Linux-i686 and Windows-i686). Those are required to create standalone build (see OZ_STANDALONE cmake
option) that can be distributed in a ZIP archive (i.e. no installation required, all dependencies
included).

This script is currently Arch Linux-specific and assumes one has all 64-bit, 32-bit (`lib32-*`) and
MinGW (`mingw32-*`) versions of all dependent libraries installed. Many of those packages must be
built from AUR.

### `nacl-test.sh` ###

Prepare directory `build/NaCl-test` (creating symlinks to binaries, compiled data archives, HTML
pages ...) for web-based, NaCl port, and launches chromium browser at `localhost:8000`. It assumes
one has already a simple web server running at port 8000, serving `build/NaCl-test` directory (e.g.
go to `build/NaCl-test` and run "`python -m http.server`").

### `package.sh [src | data]` ###

Pack source tree into `openzone-src-<version>.tar.xz` and pack compiled game data archives from
`share/*.zip` into `openzone-data-<version>.tar.xz`.

### `ports.sh` ###

This script is used to build libraries required by OpenZone for some platforms. Currently it builds
zlib, physfs, SDL, SDL_ttf, OpenAL Soft, squish for NaCl-x86_64 and NaCl-i686.
One of the following parameters must be given (`build` is assumed if run without parameters):

    * `clean`: Delete all built libraries for all platforms in `ports` directory. Downloaded sources
      are left intact.
    * `fetch`: Download sources into `ports/archives` directory.
    * `build`: Builds all libraries for all platforms.

### `q3map2.sh <map_file>` ###

Proxy script for invoking `q3map2` BSP compiler from GtkRadiant in `/usr/share/gtkradiant/` with
appropriate parameters to compile BSP structures for OpenZone.


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
