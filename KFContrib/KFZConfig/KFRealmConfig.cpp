#include "KFRealmConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFRealmConfig::ReadSetting( KFNode& xmlnode, KFRealmSeting* kfsetting )
    {
        kfsetting->_str_consume = xmlnode.GetString( "Consume", true );

        KFRealmLevel leveldata;
        leveldata._level = xmlnode.GetUInt32( "Level" );
        leveldata._explore_id = xmlnode.GetUInt32( "ExploreId" );

        auto strdropvictory = xmlnode.GetString( "DropVictory", true );
        KFReadSetting::ParseConditionList( strdropvictory, leveldata._victory_drop_list );

        auto strdropfail = xmlnode.GetString( "DropFail", true );
        KFReadSetting::ParseConditionList( strdropfail, leveldata._fail_drop_list );

        leveldata._createion = xmlnode.GetUInt32( "Creation" );
        leveldata._inner_world = xmlnode.GetUInt32( "InnerWorld" );

        kfsetting->_levels.emplace( leveldata._level, leveldata );
    }

    void KFRealmConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, kfsetting->_str_consume, __FILE__, kfsetting->_id );
        }
    }
}