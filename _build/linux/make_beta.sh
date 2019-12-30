#!/bin/bash
respath=/home/server/project/chess/trunk/Resource
configpath=../../_bin/config

svn up $respath
mkdir -p $configpath
\cp -rf $respath/config/*.xml $configpath/
\cp -rf $respath/config/Beta/*.xml $configpath/

sh make_version.sh 1.1 beta debug beta
