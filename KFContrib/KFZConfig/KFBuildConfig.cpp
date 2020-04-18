#include "KFBuildConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    const KFBuildLevelSetting* KFBuildConfig::FindBuildLevelSetting( uint32 id, uint32 level )
    {
        auto kfsetting = _settings.Find( id );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return kfsetting->_build_level_settings.Find( level );
    }

    void KFBuildConfig::ReadSetting( KFNode& xmlnode, KFBuildSetting* kfsetting )
    {
        auto level = xmlnode.GetUInt32( "Level" );
        auto kflevelsetting = kfsetting->_build_level_settings.Create( level );
        kflevelsetting->_level = level;
        if ( kfsetting->_max_level < level )
        {
            kfsetting->_max_level = level;
        }

        auto strcondition = xmlnode.GetString( "Condition" );
        kflevelsetting->_condition_type = KFReadSetting::ParseConditionList( strcondition, kflevelsetting->_condition );

        auto strtechnology = xmlnode.GetString( "Technology" );
        KFUtility::SplitList( kflevelsetting->_technology, strtechnology, __SPLIT_STRING__ );

        kflevelsetting->_upgrade_time = xmlnode.GetUInt32( "UpgradeTime", true );
        kflevelsetting->_unit_time = xmlnode.GetUInt32( "UnitTime", true );

        kflevelsetting->_consume._str_element = xmlnode.GetString( "Consume", true );
        kflevelsetting->_onekey_consume._str_element = xmlnode.GetString( "OnekeyConsume", true );
    }

    void KFBuildConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            for ( auto& leveliter : kfsetting->_build_level_settings._objects )
            {
                auto kflevelsetting = leveliter.second;
                KFElementConfig::Instance()->ParseElement( kflevelsetting->_consume, kflevelsetting->_consume._str_element, __FILE__, kfsetting->_id );
                KFElementConfig::Instance()->ParseElement( kflevelsetting->_onekey_consume, kflevelsetting->_onekey_consume._str_element, __FILE__, kfsetting->_id );
            }
        }
    }
}