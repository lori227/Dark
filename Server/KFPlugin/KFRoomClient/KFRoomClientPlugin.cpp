#include "KFRoomClientPlugin.hpp"
#include "KFRoomClientModule.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRoomClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRoomClient );
    }

    void KFRoomClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFRoomClient );
    }

    void KFRoomClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}