#include "KFSmithyPlugin.hpp"
#include "KFSmithyModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFSmithyPlugin::Install()
    {
        __REGISTER_MODULE__( KFSmithy );
    }

    void KFSmithyPlugin::UnInstall()
    {
        __UN_MODULE__( KFSmithy );
    }

    void KFSmithyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_record_client, KFRecordClientInterface );
    }

    void KFSmithyPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFSmithyConfig );
        __KF_ADD_CONFIG__( KFSmithyWeaponConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFRewardConfig );
        __KF_ADD_CONFIG__( KFItemConfig );
    }
}