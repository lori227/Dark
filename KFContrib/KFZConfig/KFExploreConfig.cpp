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
    }
}