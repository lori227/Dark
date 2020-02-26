#include "KFRealmConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFRealmConfig::ReadSetting( KFNode& xmlnode, KFRealmSeting* kfsetting )
    {
        kfsetting->_str_consume = xmlnode.GetString( "Consume", true );

        // 惩罚
        auto strpunish = xmlnode.GetString( "Punish" );
        while ( !strpunish.empty() )
        {
            auto strdata = KFUtility::SplitString( strpunish, __SPLIT_STRING__ );
            if ( !strdata.empty() )
            {
                auto itemid = KFUtility::SplitValue<uint32>( strdata, "," );
                auto weight = KFUtility::SplitValue<uint32>( strdata, "," );
                kfsetting->_punish_list[ itemid ] = __MIN__( weight, KFRandEnum::TenThousand );
            }
        }

        KFRealmLevel leveldata;
        leveldata._level = xmlnode.GetUInt32( "Level" );
        leveldata._explore_id = xmlnode.GetUInt32( "ExploreId" );

        leveldata._food_num = xmlnode.GetUInt32( "FoodNum", true );
        leveldata._food_step = xmlnode.GetUInt32( "FoodStep", true );
        leveldata._hp_num = xmlnode.GetUInt32( "HpNum", true );
        leveldata._hp_step = xmlnode.GetUInt32( "HpStep", true );

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