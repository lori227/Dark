#include "KFRoomShardPlugin.hpp"
#include "KFRoomShardModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRoomShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFRoomShard );
    }

    void KFRoomShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRoomShard );
    }

    void KFRoomShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}
