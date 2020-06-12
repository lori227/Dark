#!/bin/bash
respath=/home/server/project/chess/trunk/Resource/config
configpath=../../_bin/config

svn up $respath/server
mkdir -p $configpath
\cp -rf $respath/*.xml $configpath/
\cp -rf $respath/server/*.xml $configpath/

sh make_version.sh 2.1 develop debug trunk