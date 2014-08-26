#!/bin/bash

set -e

test ! -d debian
mkdir debian

dch --create -v "${TRAVIS_TAG}-1" --package leosac "Travis auto-generated package" # Initial changelog
dch -r "Unforeseen Consequences" # Mark as release
echo "9" > debian/compat
cp pkg/deb/control      debian
cp pkg/deb/rules        debian
cp pkg/deb/copyright    debian
cp pkg/deb/postinst     debian
mkdir debian/source
echo "3.0 (quilt)" > debian/source/format

debuild -us -uc
