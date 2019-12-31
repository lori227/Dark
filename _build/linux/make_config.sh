#!/bin/bash

respath=/home/server/project/chess/trunk/Resource/config
configpath=../../_bin/config

svn up $respath/DesignerConfig1
mkdir -p $configpath
\cp -rf $respath/*.xml $configpath/
\cp -rf $respath/DesignerConfig1/*.xml $configpath/

sh make_version.sh 1.1 config debug trunk
