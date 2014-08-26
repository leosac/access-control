#!/bin/bash

set -e
set -x

test ! -d debian
mkdir debian

dch --create -v "${TRAVIS_TAG}-1" --package leosac "Travis auto-generated package (commit ${TRAVIS_COMMIT})" # Initial changelog
dch -r "Unforeseen Consequences" # Mark as release
cat debian/changelog
echo "9" > debian/compat
cp pkg/deb/control      debian
cp pkg/deb/rules        debian
cp pkg/deb/copyright    debian
cp pkg/deb/postinst     debian
mkdir debian/source
echo "3.0 (quilt)" > debian/source/format

debuild -us -uc
