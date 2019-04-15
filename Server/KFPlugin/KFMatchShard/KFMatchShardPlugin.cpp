#include "KFMatchShardPlugin.hpp"
#include "KFMatchShardModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMatchShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFMatchShard );
    }

    void KFMatchShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMatchShard );
    }

    void KFMatchShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}