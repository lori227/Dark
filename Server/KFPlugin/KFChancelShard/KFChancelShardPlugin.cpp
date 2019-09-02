#include "KFChancelShardPlugin.hpp"
#include "KFChancelShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFChancelShardPlugin::Install ()
    {
        __REGISTER_MODULE__( KFChancelShard );
    }

    void KFChancelShardPlugin::UnInstall ()
    {
        __UN_MODULE__( KFChancelShard );
    }

    void KFChancelShardPlugin::LoadModule ()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}