#include "KFTLogPlugin.hpp"
#include "KFTLogModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTLogPlugin::Install()
    {
        __REGISTER_MODULE__( KFTLog );
    }

    void KFTLogPlugin::UnInstall()
    {
        __UN_MODULE__( KFTLog );
    }

    void KFTLogPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_project, KFProjectInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }

    void KFTLogPlugin::AddConfig()
    {
        //__KF_ADD_CONFIG__( KFTLogConfig );
    }
}