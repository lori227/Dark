#include "KFRecordShardPlugin.hpp"
#include "KFRecordShardModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRecordShardPlugin::Install ()
    {
        __REGISTER_MODULE__( KFRecordShard );
    }

    void KFRecordShardPlugin::UnInstall ()
    {
        __UN_MODULE__( KFRecordShard );
    }

    void KFRecordShardPlugin::LoadModule ()
    {
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
        __FIND_MODULE__( _kf_notice, KFNoticeInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }

    void KFRecordShardPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRecordConfig );
    }
}