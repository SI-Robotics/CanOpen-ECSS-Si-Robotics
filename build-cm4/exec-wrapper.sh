#!/bin/sh

SCRIPT_EXEC=
BINFMT_EXEC=qemu-arm-static

[ -x "$1" ] || exit $?
if [ "$(head -c 2 $1)" = "#!" ]; then
    exec $SCRIPT_EXEC "$@" ${TEST_ARGS}
else
    exec $BINFMT_EXEC "$@" ${TEST_ARGS}
fi
