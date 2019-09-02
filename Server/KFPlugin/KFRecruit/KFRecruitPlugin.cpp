#include "KFRecruitPlugin.hpp"
#include "KFRecruitModule.hpp"
#include "KFConfig/KFConfigInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRecruitPlugin::Install()
    {
        __REGISTER_MODULE__( KFRecruit );
    }

    void KFRecruitPlugin::UnInstall()
    {
        __UN_MODULE__( KFRecruit );
    }

    void KFRecruitPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_generate, KFGenerateInterface );
    }

    void KFRecruitPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRefreshConfig );
        __KF_ADD_CONFIG__( KFDivisorConfig );
        __KF_ADD_CONFIG__( KFProfessionConfig );
    }
}