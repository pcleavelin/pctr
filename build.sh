#!/bin/bash

g++ src/main.cc src/pctr.cc -Ithird_party/v8/include \
-Wl,--start-group third_party/v8/out/x64.release/obj/{libv8_{base,libbase,external_snapshot,libplatform,libsampler},\
third_party/icu/libicu{uc,i18n},src/inspector/libinspector}.a -Wl,--end-group \
-lrt -ldl -pthread -g -o target/pctr_c/pctr

./build_ts.sh