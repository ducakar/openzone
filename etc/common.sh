#!/bin/bash

# Common functions.

header_msg() {
  echo -ne "\e[1;32m"
  echo "================================================================================"
  echo
  echo "          $*"
  echo
  echo "--------------------------------------------------------------------------------"
  echo -ne "\e[0m"
}
