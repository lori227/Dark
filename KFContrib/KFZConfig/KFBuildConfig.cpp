#include "KFBuildConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    uint32 KFBuildConfig::GetSettingId( uint32 id, uint32 level )
    {
        return id * 1000u + level;
    }

    const KFBuildSetting* KFBuildConfig::FindBuildSetting( uint32 id, uint32 level )
    {
        auto settingid = GetSettingId( id, level );
        return FindSetting( settingid );
    }

    void KFBuildConfig::ReadSetting( KFNode& xmlnode, KFBuildSetting* kfsetting )
    {
        kfsetting->_build_id = xmlnode.GetUInt32( "BuildId" );
        kfsetting->_level = xmlnode.GetUInt32( "Level" );

        auto id = GetSettingId( kfsetting->_build_id, kfsetting->_level );
        if ( id != kfsetting->_id )
        {
            __LOG_ERROR__( "id=[{}] build config id is error", kfsetting->_id );
        }

        auto strcondition = xmlnode.GetString( "Condition" );
        kfsetting->_condition_type = KFReadSetting::ParseConditionList( strcondition, kfsetting->_condition );

        auto strtechnology = xmlnode.GetString( "Technology" );
        KFUtility::SplitList( kfsetting->_technology, strtechnology, __AND_STRING__ );

        kfsetting->_upgrade_time = xmlnode.GetUInt32( "UpgradeTime", true );
        kfsetting->_unit_time = xmlnode.GetUInt32( "UnitTime", true );

        kfsetting->_consume_str = xmlnode.GetString( "Consume", true );
        kfsetting->_onekey_consume_str = xmlnode.GetString( "OnekeyConsume", true );
    }

    void KFBuildConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_consume_str, kfsetting->_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] build config consume is error", kfsetting->_id );
                }
            }

            if ( !kfsetting->_onekey_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_onekey_consume_str, kfsetting->_onekey_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] build config onekey consume is error", kfsetting->_id );
                }
            }
        }
    }
}