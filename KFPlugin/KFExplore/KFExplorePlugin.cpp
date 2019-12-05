#include "KFExplorePlugin.hpp"
#include "KFExploreModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFExplorePlugin::Install()
    {
        __REGISTER_MODULE__( KFExplore );
    }

    void KFExplorePlugin::UnInstall()
    {
        __UN_MODULE__( KFExplore );
    }

    void KFExplorePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_item, KFItemInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_generate, KFGenerateInterface );
        __FIND_MODULE__( _kf_hero_team, KFHeroTeamInterface );
    }

    void KFExplorePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFPVEConfig );
        __KF_ADD_CONFIG__( KFExploreConfig );
        __KF_ADD_CONFIG__( KFFoodConsumeConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFCurrencyConfig );
        __KF_ADD_CONFIG__( KFItemConfig );
    }
}