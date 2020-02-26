#include "KFBuildPlugin.hpp"
#include "KFBuildModule.hpp"
#include "KFTechnologyModule.hpp"
#include "KFConfig/KFConfigInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBuildPlugin::Install()
    {
        __REGISTER_MODULE__( KFBuild );
        __REGISTER_MODULE__( KFTechnology );
    }

    void KFBuildPlugin::UnInstall()
    {
        __UN_MODULE__( KFBuild );
        __UN_MODULE__( KFTechnology );
    }

    void KFBuildPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
    }

    void KFBuildPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFBuildConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFTechnologyConfig );
    }

}