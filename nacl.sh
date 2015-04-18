#!/bin/sh
#
# nacl.sh [run | debug | finalise | translate]
#
# Linux-x86_64-Clang client is launched by default. <options> are passed to the client command line.
# `NACL_SDK_ROOT` environment variable must be set to use this script.
#
# The following alternative launches are available:
#
# - `run`: creates symlinks to compiled data archives, HTML pages etc. in the target directory,
#   starts a simple python web server in that directory at port 8000 (`python -m http.server`) and
#   opens `localhost:8000` in chromium browser to test the web-based NaCl port.
# - `debug`: starts gdb and connets it to a running Chromium instance with a NaCl module pending for
#   debugging.
# - `finalise`: runs `pnacl-finalize` to finalise openzone PNaCl pexe executable.
# - `translate`: translates openzone PNaCl pexe to host machine architecture's nexe.
#

if [[ `uname -m` == x86_64 ]]; then
  arch=x86_64
  bits=64
else
  arch=i686
  bits=32
fi

chromium="/usr/bin/chromium"
pnaclPath="${NACL_SDK_ROOT}/toolchain/linux_pnacl/bin${bits}"
naclPath="${NACL_SDK_ROOT}/toolchain/linux_x86_newlib/bin"

function run()
{
  mkdir -p build/PNaCl/src/tools

  # Just create symlinks instead of copying.
  for i in share/openzone/*.{7z,zip} share/openzone/packages.ozManifest \
           doc etc/nacl/openzone.??.html etc/nacl/nacl.js etc/nacl/openzone.nmf
  do
    [[ -e $i ]] && ln -sf ../../../../$i build/PNaCl/src/tools
  done

  cd build/PNaCl/src/tools
  python3 -m http.server &
  serverPID=$!

  sleep 3
  ${chromium} --user-data-dir="$HOME/.config/chromium-test" \
	      http://localhost:8000/openzone.sl.html || true

  kill $serverPID
}

debug()
{
  "$naclPath/${arch}-nacl-gdb" -ex 'target remote localhost:4014' \
			       build/PNaCl/src/tools/openzone.${arch}.nexe
}

finalise()
{
  echo Finalising ...
  "$pnaclPath/pnacl-finalize" build/PNaCl/src/tools/openzone.pexe
}

translate()
{
  echo Translating ...
  "$pnaclPath/pnacl-translate" --allow-llvm-bitcode-input build/PNaCl/src/tools/openzone.pexe \
			       -arch $arch -o build/PNaCl/src/tools/openzone.${arch}.nexe
}

case $1 in
  run)
    run
    ;;
  debug)
    debug
    ;;
  finalise)
    finalise
    ;;
  translate)
    translate
    ;;
esac
