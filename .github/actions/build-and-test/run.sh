#!/bin/bash
echo "----- All env-vars -----"
env
echo "----- All env-vars -----"
echo
if [ "$TEDITOR_BUILD_DOCS" = "1" ]; then
    make doc
else
    make -j4 \
         CURL_OPTS=-k \
         DEBUG=$TEDITOR_DEBUG \
         VERBOSE=1 \
         teditor tests
fi
