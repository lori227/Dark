﻿#include "KFHeroPlugin.hpp"
#include "KFHeroModule.hpp"
#include "KFHeroTeamModule.hpp"
#include "KFTransferModule.hpp"
#include "KFContractModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFHeroPlugin::Install()
    {
        __REGISTER_MODULE__( KFHero );
        __REGISTER_MODULE__( KFHeroTeam );
        __REGISTER_MODULE__( KFTransfer );
        __REGISTER_MODULE__( KFContract );
    }

    void KFHeroPlugin::UnInstall()
    {
        __UN_MODULE__( KFHero );
        __UN_MODULE__( KFHeroTeam );
        __UN_MODULE__( KFTransfer );
        __UN_MODULE__( KFContract );
    }

    void KFHeroPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_filter, KFFilterInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_generate, KFGenerateInterface );
        __FIND_MODULE__( _kf_realm, KFRealmInterface );
        __FIND_MODULE__( _kf_pve, KFPVEInterface );
    }

    void KFHeroPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFLevelConfig );
        __KF_ADD_CONFIG__( KFProfessionConfig );
        __KF_ADD_CONFIG__( KFTransferConfig );
        __KF_ADD_CONFIG__( KFWeightConfig );
        __KF_ADD_CONFIG__( KFSkillConfig );
        __KF_ADD_CONFIG__( KFCharacterConfig );
        __KF_ADD_CONFIG__( KFRaceConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFFormulaConfig );
        __KF_ADD_CONFIG__( KFInjuryRandomConfig );
        __KF_ADD_CONFIG__( KFQualityConfig );
    }
}
