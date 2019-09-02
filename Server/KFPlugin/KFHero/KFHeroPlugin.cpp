#include "KFHeroPlugin.hpp"
#include "KFHeroModule.hpp"
#include "KFMarshalModule.hpp"
#include "KFTransferModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHeroPlugin::Install()
    {
        __REGISTER_MODULE__( KFHero );
        __REGISTER_MODULE__( KFMarshal );
        __REGISTER_MODULE__( KFTransfer );
    }

    void KFHeroPlugin::UnInstall()
    {
        __UN_MODULE__( KFHero );
        __UN_MODULE__( KFMarshal );
        __UN_MODULE__( KFTransfer );
    }

    void KFHeroPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_generate, KFGenerateInterface );
    }

    void KFHeroPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFLevelConfig );
        __KF_ADD_CONFIG__( KFProfessionConfig );
        __KF_ADD_CONFIG__( KFTransferConfig );
        __KF_ADD_CONFIG__( KFEquipTypeConfig );
        __KF_ADD_CONFIG__( KFWeightConfig );
        __KF_ADD_CONFIG__( KFSkillConfig );
        __KF_ADD_CONFIG__( KFCharacterConfig );
    }
}
