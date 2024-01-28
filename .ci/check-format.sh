#!/usr/bin/env bash

CLANG_FORMATER=clang-format
CLANG_FORMATER_FLAGS=-style=file

# check clang-format installed
command -v "$CLANG_FORMATER" > /dev/null
ret=$?

if [[ $ret -ne 0 ]]; then
    echo -e "\e[31;1m$CLANG_FORMATER not installed...\e[0m"
    exit $ret
else
    # make sure version >= 12
    $CLANG_FORMATER --version | grep -P ".*version (1[2-9]|[2-9]\d+).*" >> /dev/null
    ret=$?

    if [[ $ret -ne 0 ]]; then
        echo -e "\e[31;1m$CLANG_FORMATER version should >= 12 ...\e[0m"
        exit $ret
    fi
fi

# check format (based on sysprog21/lab0-c/.ci/check-format.sh)

SOURCES=$(find $(git rev-parse --show-toplevel) | egrep "\.(c|h)\$")

for file in ${SOURCES};
do
    $CLANG_FORMATER $CLANG_FORMATER_FLAGS ${file} > expected-format
    diff -u -p --label="${file}" --label="expected coding style" ${file} expected-format
done
exit $($CLANG_FORMATER --output-replacements-xml ${SOURCES} | egrep -c "</replacement>")