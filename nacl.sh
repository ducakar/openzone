#!/bin/sh
#
# nacl.sh [run | debug | manifest]
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
# - `manifest`: writes `share/openzone/manifest.json` file that contains list of game packeges
#   together with their timestamps. Needed by NaCl to update cached game packages.
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
  for i in share/openzone/*.{7z,zip,json} doc etc/nacl/openzone.??.html etc/nacl/nacl.js \
           etc/nacl/openzone.nmf
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

function manifest()
{
  cd share/openzone

  printf '{\n' > manifest.json

  first=1

  for pkg in `echo *.zip`; do
    if [[ -f $pkg ]]; then
      timestamp=`stat -c %Y $pkg`

      (( $first )) && first=0 || ( printf ',\n' >> manifest.json )
      printf "  \"$pkg\": $timestamp" >> manifest.json
    fi
  done

  printf '\n}\n' >> manifest.json

  cat manifest.json
}

case $1 in
  run)
    run
    ;;
  debug)
    debug
    ;;
  manifest)
    manifest
    ;;
esac
