#!/bin/sh

find . -regex '.*\.svn$' -exec rm -rf '{}' \; &> /dev/null
