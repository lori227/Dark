#include "KFMainCampPlugin.hpp"
#include "KFMainCampModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFMainCampPlugin::Install()
    {
        __REGISTER_MODULE__( KFMainCamp );
    }

    void KFMainCampPlugin::UnInstall()
    {
        __UN_MODULE__( KFMainCamp );
    }

    void KFMainCampPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
    }

    void KFMainCampPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFBuildLevelConfig );
        __KF_ADD_CONFIG__( KFBuildSkinConfig );
    }

}