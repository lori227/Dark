#include "KFGeneratePlugin.hpp"
#include "KFGenerateModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGeneratePlugin::Install()
    {
        __REGISTER_MODULE__( KFGenerate );
    }

    void KFGeneratePlugin::UnInstall()
    {
        __UN_MODULE__( KFGenerate );
    }

    void KFGeneratePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_item, KFItemInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
    }

    void KFGeneratePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFGenerateConfig );
        __KF_ADD_CONFIG__( KFWeightConfig );
        __KF_ADD_CONFIG__( KFRaceConfig );
        __KF_ADD_CONFIG__( KFCharacterConfig );
        __KF_ADD_CONFIG__( KFBackGroundConfig );
        __KF_ADD_CONFIG__( KFHeroAttributeConfig );
        __KF_ADD_CONFIG__( KFProfessionConfig );
        __KF_ADD_CONFIG__( KFSkillConfig );
        __KF_ADD_CONFIG__( KFPinchFaceConfig );
        __KF_ADD_CONFIG__( KFDivisorConfig );
        __KF_ADD_CONFIG__( KFNpcConfig );
        __KF_ADD_CONFIG__( KFNpcSkillConfig );
        __KF_ADD_CONFIG__( KFLevelConfig );
        __KF_ADD_CONFIG__( KFItemConfig );
        __KF_ADD_CONFIG__( KFTransferConfig );
        __KF_ADD_CONFIG__( KFWeaponTypeConfig );
        __KF_ADD_CONFIG__( KFQualityConfig );
        __KF_ADD_CONFIG__( KFInjuryConfig );
    }

}