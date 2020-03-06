#include "KFPVEConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFPVEConfig::ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting )
    {
        kfsetting->_npc_rand_id = xmlnode.GetUInt32( "NpcRand" );

        auto strdungeon = xmlnode.GetString( "DungeonId" );
        KFUtility::SplitList( kfsetting->_dungeon_id_list, strdungeon, __SPLIT_STRING__ );

        auto strdropvictory = xmlnode.GetString( "DropVictory", true );
        KFReadSetting::ParseConditionList( strdropvictory, kfsetting->_victory_drop_list );

        auto strinnerdropvictory = xmlnode.GetString( "InnerDropVictory", true );
        KFReadSetting::ParseConditionList( strinnerdropvictory, kfsetting->_inner_victory_drop_list );

        auto strdropfail = xmlnode.GetString( "DropFail", true );
        KFReadSetting::ParseConditionList( strdropfail, kfsetting->_fail_drop_list );

        auto strinnerdropfail = xmlnode.GetString( "InnerDropFail", true );
        KFReadSetting::ParseConditionList( strinnerdropfail, kfsetting->_inner_fail_drop_list );

        kfsetting->_str_consume = xmlnode.GetString( "Consume", true );

        kfsetting->_normal_flee_rate = xmlnode.GetUInt32( "NormalFlee" );
        kfsetting->_inner_flee_rate = xmlnode.GetUInt32( "InnerFlee" );
        kfsetting->_level_coefficient = xmlnode.GetUInt32( "LevelCoefficient" );
        kfsetting->_punish_id = xmlnode.GetUInt32( "PunishId" );
    }

    void KFPVEConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, kfsetting->_str_consume, __FILE__, kfsetting->_id );

            kfsetting->_npc_rand_setting = KFNpcRandConfig::Instance()->FindSetting( kfsetting->_npc_rand_id );
            if ( kfsetting->_npc_rand_setting == nullptr )
            {
                __LOG_ERROR__( "pve=[{}] npcrandid=[{}] can't find setting", kfsetting->_id, kfsetting->_npc_rand_id );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}