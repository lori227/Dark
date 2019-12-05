#!/bin/bash

respath=../../../Resource
configpath=../../_bin/config

svn up $respath
mkdir -p $configpath
\cp -rf $respath/config/*.xml $configpath/
\cp -rf $respath/config/DesignerConfig1/*.xml $configpath/

sh make_version.sh 1.2 develop debug $1 $2
