#include "KFChancelClientPlugin.hpp"
#include "KFChancelClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFChancelClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFChancelClient );
    }

    void KFChancelClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFChancelClient );
    }

    void KFChancelClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
    }

    void KFChancelClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFChancelConfig );
        __KF_ADD_CONFIG__( KFProfessionConfig );
    }
}