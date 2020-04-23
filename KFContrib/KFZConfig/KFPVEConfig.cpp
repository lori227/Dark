﻿#include "KFPVEConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFPVEConfig::ReadSetting( KFNode& xmlnode, KFPVESetting* kfsetting )
    {
        kfsetting->_npc_rand_id = xmlnode.GetUInt32( "NpcRand" );

        kfsetting->_dungeon_id_list = xmlnode.GetUInt32Vector( "DungeonId" );

        auto strdropvictory = xmlnode.GetString( "DropVictory", true );
        KFReadSetting::ParseConditionList( strdropvictory, kfsetting->_victory_drop_list );

        auto strinnerdropvictory = xmlnode.GetString( "InnerDropVictory", true );
        KFReadSetting::ParseConditionList( strinnerdropvictory, kfsetting->_inner_victory_drop_list );

        auto strdropfail = xmlnode.GetString( "DropFail", true );
        KFReadSetting::ParseConditionList( strdropfail, kfsetting->_fail_drop_list );

        auto strinnerdropfail = xmlnode.GetString( "InnerDropFail", true );
        KFReadSetting::ParseConditionList( strinnerdropfail, kfsetting->_inner_fail_drop_list );

        kfsetting->_consume._str_parse = xmlnode.GetString( "Consume", true );

        kfsetting->_normal_flee_rate = xmlnode.GetUInt32( "NormalFlee" );
        kfsetting->_inner_flee_rate = xmlnode.GetUInt32( "InnerFlee" );
        kfsetting->_level_coefficient = xmlnode.GetUInt32( "LevelCoefficient" );
        kfsetting->_punish_id = xmlnode.GetUInt32( "PunishId" );
        kfsetting->_durability = xmlnode.GetUInt32( "Durability", true );
        kfsetting->_time_id = xmlnode.GetUInt32( "PveTime", true );
        kfsetting->_weather_id = xmlnode.GetUInt32( "Weather", true );
    }

    void KFPVEConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, __FILE__, kfsetting->_id );

            kfsetting->_npc_rand_setting = KFNpcRandConfig::Instance()->FindSetting( kfsetting->_npc_rand_id );
            if ( kfsetting->_npc_rand_setting == nullptr )
            {
                __LOG_ERROR__( "pve=[{}] npcrandid=[{}] can't find setting", kfsetting->_id, kfsetting->_npc_rand_id );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}