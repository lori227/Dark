#ifndef __KF_RECORD_SHARD_INTERFACE_H__
#define __KF_RECORD_SHARD_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRecordShardInterface : public KFModule
    {

    };

    //////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__ ( _kf_record_shared, KFRecordShardInterface );
    //////////////////////////////////////////////////////////////////////////////////
}

#endif