#include "KFRealmPlugin.hpp"
#include "KFPVEModule.hpp"
#include "KFRealmModule.hpp"
#include "KFGambleModule.hpp"
#include "KFBuffModule.hpp"
#include "KFRealmTimeModule.hpp"
#include "KFWeatherModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRealmPlugin::Install()
    {
        __REGISTER_MODULE__( KFPVE );
        __REGISTER_MODULE__( KFBuff );
        __REGISTER_MODULE__( KFRealm );
        __REGISTER_MODULE__( KFGamble );
        __REGISTER_MODULE__( KFWeather );
        __REGISTER_MODULE__( KFRealmTime );
    }

    void KFRealmPlugin::UnInstall()
    {
        __UN_MODULE__( KFPVE );
        __UN_MODULE__( KFBuff );
        __UN_MODULE__( KFRealm );
        __UN_MODULE__( KFGamble );
        __UN_MODULE__( KFWeather );
        __UN_MODULE__( KFRealmTime );
    }

    void KFRealmPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_drop, KFDropInterface );
        __FIND_MODULE__( _kf_hero, KFHeroInterface );
        __FIND_MODULE__( _kf_item, KFItemInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_execute, KFExecuteInterface );
        __FIND_MODULE__( _kf_generate, KFGenerateInterface );
        __FIND_MODULE__( _kf_hero_team, KFHeroTeamInterface );
        __FIND_MODULE__( _kf_realm, KFRealmInterface );
        __FIND_MODULE__( _kf_item_move, KFItemMoveInterface );
        __FIND_MODULE__( _kf_pve, KFPVEInterface );
    }

    void KFRealmPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFPVEConfig );
        __KF_ADD_CONFIG__( KFRealmConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFItemConfig );
        __KF_ADD_CONFIG__( KFItemTypeConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
        __KF_ADD_CONFIG__( KFPunishConfig );
        __KF_ADD_CONFIG__( KFNpcGroupConfig );
        __KF_ADD_CONFIG__( KFNpcRandConfig );
        __KF_ADD_CONFIG__( KFInnerWorldConfig );
        __KF_ADD_CONFIG__( KFGambleConfig );
        __KF_ADD_CONFIG__( KFExchangeConfig );
        __KF_ADD_CONFIG__( KFSelectConfig );
        __KF_ADD_CONFIG__( KFMultiEventConfig );
        __KF_ADD_CONFIG__( KFLevelValueConfig );
        __KF_ADD_CONFIG__( KFRealmTimeConfig );
        __KF_ADD_CONFIG__( KFWeatherConfig );
        __KF_ADD_CONFIG__( KFItemBagConfig );
    }
}