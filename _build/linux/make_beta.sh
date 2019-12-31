#!/bin/bash
respath=/home/server/project/chess/trunk/Resource/config
configpath=../../_bin/config

svn up $respath/Beta
mkdir -p $configpath
\cp -rf $respath/*.xml $configpath/
\cp -rf $respath/Beta/*.xml $configpath/

sh make_version.sh 1.1 beta debug beta
