#!/bin/bash
## 
## We use the static-pages for hand-crafted HTML content.
## We want to keep history of this static-pages branch.

## The gh-pages contains the full website: hand-crafted HTML + generated
## documentation. We are not interrested in the history of this branch:
##    * The repo will get fat. Fast.
##    * Since the documentation is generated, we can generated previous
##      version of the documentation manually.

set -e
set -x

REPO_DIRECTORY=/home/xaqq/Documents/leosac
cd $REPO_DIRECTORY

## Generate documentation tarball from the current tree.
git checkout develop
doxygen && tar cvf /tmp/leosac_develop_doc.tar -C doc/html .
tar cvf /tmp/leosac_develop_doc_rc.tar -C doc/resources .

## Create a new gh-pages from the static-pages tree. 
git checkout static-pages
git branch -D gh-pages
git checkout -b gh-pages

BRANCH=develop

## Extract the previous tarball.
mkdir -p doc_output/$BRANCH/html/ doc_output/$BRANCH/resources
tar xvf /tmp/leosac_develop_doc.tar -C doc_output/$BRANCH/html/
tar xvf /tmp/leosac_develop_doc_rc.tar -C doc_output/$BRANCH/resources
git add doc_output/$BRANCH
git commit -m "Adding generated documentation"
git push origin gh-pages -f


echo "Success!"
