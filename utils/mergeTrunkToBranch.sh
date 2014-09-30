#!/bin/sh

#URL: https://qt-flac.googlecode.com/svn/trunk
#Repository Root: https://qt-flac.googlecode.com/svn
#Repository UUID: 7097bc46-167c-b9cb-eaf3-56d25b1c32fc

URL=https://qt-flac.googlecode.com/svn
NAME=05_07_2014

rsync -r -v -p --exclude ".svn" $URL/trunk/ $URL/branch/$NAME
