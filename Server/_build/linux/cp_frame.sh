#!bash/sh

# lib
libpath=../../_lib/linux
rm -rf $libpath
mkdir -p "$libpath"

framelibpath=../../../../../../frame/trunk/_lib/linux
cp -rf $framelibpath/* $libpath/ 


# bin
binpath=../../_bin/bin/linux/$1
mkdir -p "$binpath"
rm -f $binpath/*

filename=""
if [ $1 == "debug" ];then
	filename="d"
fi

framebinpath=../../../../../../frame/trunk/_bin/bin/linux/$1
cp -f $framebinpath/KFStartup$filename $binpath/
cp -f $framebinpath/KFConfig$filename.so $binpath/
cp -f $framebinpath/KFMySQL$filename.so $binpath/
cp -f $framebinpath/KFRedis$filename.so $binpath/
cp -f $framebinpath/KFDeployClient$filename.so $binpath/
cp -f $framebinpath/KFClusterClient$filename.so $binpath/
cp -f $framebinpath/KFClusterMaster$filename.so $binpath/
cp -f $framebinpath/KFClusterProxy$filename.so $binpath/
cp -f $framebinpath/KFClusterShard$filename.so $binpath/
cp -f $framebinpath/KFLogClient$filename.so $binpath/
cp -f $framebinpath/KFLogShard$filename.so $binpath/
cp -f $framebinpath/KFKernel$filename.so $binpath/
cp -f $framebinpath/KFFilter$filename.so $binpath/
cp -f $framebinpath/KFPlayer$filename.so $binpath/
cp -f $framebinpath/KFHttpClient$filename.so $binpath/
cp -f $framebinpath/KFHttpServer$filename.so $binpath/
cp -f $framebinpath/KFMessage$filename.so $binpath/
cp -f $framebinpath/KFBus$filename.so $binpath/
cp -f $framebinpath/KFTcpServer$filename.so $binpath/
cp -f $framebinpath/KFTcpClient$filename.so $binpath/
cp -f $framebinpath/KFTcpDiscover$filename.so $binpath/
cp -f $framebinpath/KFIpAddress$filename.so $binpath/
cp -f $framebinpath/KFOption$filename.so $binpath/
cp -f $framebinpath/KFRouteClient$filename.so $binpath/
cp -f $framebinpath/KFRouteProxy$filename.so $binpath/
cp -f $framebinpath/KFRouteShard$filename.so $binpath/
cp -f $framebinpath/KFSchedule$filename.so $binpath/
cp -f $framebinpath/KFTimer$filename.so $binpath/
cp -f $framebinpath/KFAuth$filename.so $binpath/
cp -f $framebinpath/KFChannel$filename.so $binpath/
cp -f $framebinpath/KFDataClient$filename.so $binpath/
cp -f $framebinpath/KFDataShard$filename.so $binpath/
cp -f $framebinpath/KFDisplay$filename.so $binpath/
cp -f $framebinpath/KFGame$filename.so $binpath/
cp -f $framebinpath/KFGate$filename.so $binpath/
cp -f $framebinpath/KFLogin$filename.so $binpath/
cp -f $framebinpath/KFWorld$filename.so $binpath/
cp -f $framebinpath/KFZone$filename.so $binpath/
cp -f $framebinpath/KFCommand$filename.so $binpath/
cp -f $framebinpath/KFEnter$filename.so $binpath/
cp -f $framebinpath/KFLeave$filename.so $binpath/
cp -f $framebinpath/KFReset$filename.so $binpath/

