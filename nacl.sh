#!/bin/bash
#
# nacl.sh [run | manifest]
#
# `NACL_SDK_ROOT` environment variable must be set to use this script.
#
# The following alternative launches are available:
#
# - `run`: creates symlinks to compiled data archives, HTML pages etc. in the target directory,
#   starts a simple python web server in that directory at port 8000 (`python -m http.server`) and
#   opens `localhost:8000` in chromium browser to test the web-based NaCl port.
# - `manifest`: writes `share/openzone/manifest.json` file that contains list of game packeges
#   together with their timestamps. Needed by NaCl to update cached game packages.
#

set -e

chromium="/usr/bin/chromium"

function run()
{
  mkdir -p build/PNaCl/src/tools

  # Just create symlinks instead of copying.
  for i in doc etc/nacl/nacl.* etc/nacl/openzone.* share/openzone/*.{zip,json}; do
    [[ -e $i ]] && ln -sf ../../../../$i build/PNaCl/src/tools
  done

  cd build/PNaCl/src/tools
  python3 -m http.server &
  serverPID=$!

  sleep 3
  $chromium "http://localhost:8000/openzone.en.html" || true

  kill $serverPID
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
