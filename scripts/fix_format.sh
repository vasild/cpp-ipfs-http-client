#!/usr/bin/env bash
find src/ include/ -iname *.h -o -iname *.cc | xargs clang-format -i -style=file -fallback-style=Google -assume-filename=../.clang-format
