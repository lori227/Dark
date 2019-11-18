#include "KFGranaryPlugin.hpp"
#include "KFGranaryModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGranaryPlugin::Install()
    {
        __REGISTER_MODULE__( KFGranary );
    }

    void KFGranaryPlugin::UnInstall()
    {
        __UN_MODULE__( KFGranary );
    }

    void KFGranaryPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_item, KFItemInterface );
        __FIND_MODULE__( _kf_record_client, KFRecordClientInterface );
    }

    void KFGranaryPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFGranaryConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
    }
}