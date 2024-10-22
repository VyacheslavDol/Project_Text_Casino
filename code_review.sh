#!/bin/bash

find . -type d -name 'lib' -prune -o -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

cppcheck --enable=all --inconclusive --suppress=missingIncludeSystem --std=c17 -i ./lib .