#ifndef __KF_CHANCEL_SHARD_INTERFACE_H__
#define __KF_CHANCEL_SHARD_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFChancelShardInterface : public KFModule
    {

    };

    //////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__ ( _kf_chancel_shared, KFChancelShardInterface );
    //////////////////////////////////////////////////////////////////////////////////
}

#endif