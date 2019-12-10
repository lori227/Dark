#include "KFExploreConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFExploreConfig::ReadSetting( KFNode& xmlnode, KFExploreSeting* kfsetting )
    {
        auto strdropvictory = xmlnode.GetString( "DropVictory", true );
        KFReadSetting::ParseConditionList( strdropvictory, kfsetting->_victory_drop_list );

        auto strdropfail = xmlnode.GetString( "DropFail", true );
        KFReadSetting::ParseConditionList( strdropfail, kfsetting->_fail_drop_list );

        kfsetting->_consume_str = xmlnode.GetString( "Consume", true );
    }

    void KFExploreConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;

            if ( !kfsetting->_consume_str.empty() )
            {
                if ( !KFRewardConfig::Instance()->ParseRewards( kfsetting->_consume_str, kfsetting->_consume ) )
                {
                    __LOG_ERROR__( "id=[{}] explore config consume is error", kfsetting->_id );
                }
            }
        }
    }
}