#!/bin/sh
#
# Create layout in build/NaCL-test directory for launching NaCl port and launch
# Chromium browser. It assumes OpenZone client (including game data) is already
# built. Local web server is estabilished (python -m http.server) to server web
# page.
# Pass "strip" as the first parameter to strip binaries.
#

# Extract path to NaCl SDK from CMake toolchain files.
nacl64Root=`egrep '^set\( PLATFORM_PREFIX' cmake/NaCl-x86_64.Toolchain.cmake | \
            sed -r 's|^set\( PLATFORM_PREFIX *"(.*)\" )|\1|'`
nacl32Root=`egrep '^set\( PLATFORM_PREFIX' cmake/NaCl-i686.Toolchain.cmake | \
            sed -r 's|^set\( PLATFORM_PREFIX *"(.*)\" )|\1|'`
prefix=`pwd`

mkdir -p build/NaCl-test

# Just create symlinks instead of copying.
for i in share/openzone/*.{7z,zip} share/openzone/packages.ozManifest \
         build/NaCl-*/src/tools/openzone.*.nexe etc/nacl/openzone.nmf \
         etc/nacl/openzone.??.html doc
do
  [[ -e "$prefix/$i" ]] && ln -sf "$prefix/$i" build/NaCl-test
done

# Strip binaries if "strip" parameter given.
if [[ $1 == strip ]]; then
  [[ -e "build/NaCl-test/openzone.x86_64.nexe" ]] && \
     "$nacl64Root/bin/x86_64-nacl-strip" "build/NaCl-test/openzone.x86_64.nexe"
  [[ -e "build/NaCl-test/openzone.i686.nexe" ]] && \
     "$nacl64Root/bin/i686-nacl-strip" "build/NaCl-test/openzone.i686.nexe"
fi

cd build/NaCl-test
python -m http.server &
serverPID=$!

sleep 3
chromium --user-data-dir="$HOME/.config/chromium-test" "http://localhost:8000/openzone.sl.html" || true

kill $serverPID
