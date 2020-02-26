#include "KFCampPlugin.hpp"
#include "KFCampModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFCampPlugin::Install()
    {
        __REGISTER_MODULE__( KFCamp );
    }

    void KFCampPlugin::UnInstall()
    {
        __UN_MODULE__( KFCamp );
    }

    void KFCampPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
    }

    void KFCampPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFBuildSkinConfig );
    }

}