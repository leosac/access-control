#!/bin/bash

set -e
set -x

test ! -d debian

DEBEMAIL="support@leosac.com"
DEBFULLNAME="Leosac CI"
export DEBEMAIL DEBFULLNAME

cp -R pkg/debian .

dch -v "${VERSION}-1" --package leosac "CI auto-generated package (commit ${VCS_REF})" # Initial changelog
dch -r "Unforeseen Consequences" # Mark as release

debuild -b -us -uc
