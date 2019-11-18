#include "KFTrainCampPlugin.hpp"
#include "KFTrainCampModule.hpp"
#include "KFConfig/KFConfigInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFTrainCampPlugin::Install()
    {
        __REGISTER_MODULE__( KFTrainCamp );
    }

    void KFTrainCampPlugin::UnInstall()
    {
        __UN_MODULE__( KFTrainCamp );
    }

    void KFTrainCampPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_record_client, KFRecordClientInterface );
    }

    void KFTrainCampPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFTrainCampConfig );
        __KF_ADD_CONFIG__( KFLevelConfig );
        __KF_ADD_CONFIG__( KFRewardConfig );
    }

}