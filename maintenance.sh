#!/bin/bash
# This script perform various simple maintenance tasks:
#     + It reformats the source code using clang-format
#     + It checks for source files that may miss license information


# Format source code
find -L src test -name "*.cpp" -or -name "*.hpp" | xargs clang-format-3.9 -i -style=file 


# Find files missing license header
for f in `find test src -name "*.cpp" -or -name "*.hpp"`
do
    # get line 2 of the files. 
    COPYRIGHT=`head -n2 $f | tail -n1`
    EXPECTED="    Copyright (C) 2014-2016 Leosac"
    if [ "$COPYRIGHT" != "$EXPECTED" ]
    then
        echo "File $f doesn't have license information."
    fi
done

# Update year of copyright info
#for f in `find test src -name "*.cpp" -or -name "*.hpp"`
#do
#    sed -i 's/Copyright (C) 2014-2015 Leosac/Copyright (C) 2014-2016 Leosac/' $f
#done
