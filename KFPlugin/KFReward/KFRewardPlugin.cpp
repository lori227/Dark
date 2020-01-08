#include "KFRewardPlugin.hpp"
#include "KFRewardModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRewardPlugin::Install()
    {
        __REGISTER_MODULE__( KFReward );
    }

    void KFRewardPlugin::UnInstall()
    {
        __UN_MODULE__( KFReward );
    }

    void KFRewardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
    }

    void KFRewardPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRewardConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
    }

}