#!/bin/bash

set -e
set -x

test ! -d debian
mkdir debian

dch --create -v "${TAG}-1" --package leosac "CI auto-generated package (commit ${COMMIT})" # Initial changelog
dch -r "Unforeseen Consequences" # Mark as release
cat debian/changelog
echo "10" > debian/compat
cp pkg/deb/control      debian
cp pkg/deb/rules        debian
cp pkg/deb/copyright    debian
cp pkg/deb/postinst     debian
mkdir debian/source
echo "3.0 (quilt)" > debian/source/format

debuild -b -us -uc
