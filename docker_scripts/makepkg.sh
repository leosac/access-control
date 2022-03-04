#!/bin/bash

set -e
set -x

test ! -d debian
mkdir debian

DEBVERSION="${VERSION}"
if [ $VERSION == "edge" ] || [ $VERSION == "latest" ] || [ $VERSION == "snapshot" ] ; then DEBVERSION=0.0.0 ; fi

dch --create -v "${DEBVERSION}-1" --package leosac "CI auto-generated package (commit ${VCS_REF})" # Initial changelog
dch -r "Unforeseen Consequences" # Mark as release
cat debian/changelog
echo "10" > debian/compat
cp pkg/debian/control      debian
cp pkg/debian/rules        debian
cp pkg/debian/copyright    debian
cp pkg/debian/postinst     debian
mkdir debian/source
echo "3.0 (quilt)" > debian/source/format

debuild -b -us -uc

LEOSAC_DEB=`find * -maxdepth 0 -name leosac_*.deb`
mv ${LEOSAC_DEB} /tmp/leosac
