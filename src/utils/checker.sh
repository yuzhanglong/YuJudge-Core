#!/bin/sh

PROGRAM="$1"
TESTOUT="$2"

diff -a "$PROGRAM" "$TESTOUT"
EXITCODE=$?

[ $EXITCODE -gt 1 ] && exit 1

exit 0